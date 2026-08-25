/* Mission logic: ground-lit booster with TVC, no fins/canards.
 * BOOST1: TVC active, gimbal corrects toward vertical.
 * COAST1 -> APOGEE1: TVC off, ballistic.
 * STAGE_DELAY: wait, then one pyro fire ignites stage 2 AND hot-separates
 * the booster in the same event (single channel, per hardware design).
 * BOOST2/COAST2/APOGEE2/DESCENT: no recovery system - vehicle free-falls
 * to LANDED. There is no deployment anywhere in this FSM by design. */
#include "flight_state.h"
#include "app_config.h"
#include "ahrs.h"
#include "kalman.h"
#include "tvc_control.h"
#include "pyro.h"
#include "safety.h"
#include <math.h>

fsm_t g_fsm;

void fsm_init(void)
{
    g_fsm.state = ST_GROUND_IDLE;
    g_fsm.state_entry_ms = HAL_GetTick();
}

static void enter(flight_state_e next)
{
    g_fsm.state = next;
    g_fsm.state_entry_ms = HAL_GetTick();
}

static uint32_t ms_in_state(uint32_t now) { return now - g_fsm.state_entry_ms; }

int fsm_request_arm(void)
{
    if (g_fsm.state != ST_GROUND_IDLE)
        return -1;
    if (!safety_check_arm_gate(&g_fsm.snapshot))
        return -1;
    if (pyro_arm() != 0)
        return -1;
    enter(ST_ARMED);
    return 0;
}

void fsm_tick(const imu_sample_t *s, float dt_s)
{
    uint32_t now = HAL_GetTick();
    att_state_t att = ahrs_get_attitude();
    alt_state_t alt = kalman_get_state();

    g_fsm.snapshot.t_ms = now;
    g_fsm.snapshot.imu = *s;
    g_fsm.snapshot.alt = alt;
    g_fsm.snapshot.att = att;
    g_fsm.snapshot.cont_ok = pyro_continuity_ok();

    float axial_g = s->az;   /* matches ARM_UP_AXIS/SIGN convention used upstream */

    switch (g_fsm.state) {

    case ST_GROUND_IDLE:
        break;   /* only fsm_request_arm() moves this to ST_ARMED */

    case ST_ARMED: {
        static uint32_t launch_thresh_start_ms = 0;
        if (axial_g > LAUNCH_G || alt.alt_m > LAUNCH_BARO_ALT_M) {
            if (launch_thresh_start_ms == 0)
                launch_thresh_start_ms = now;
            if (now - launch_thresh_start_ms >= LAUNCH_DEBOUNCE_MS) {
                launch_thresh_start_ms = 0;
                ahrs_notify_launch();
                enter(ST_BOOST1);
            }
        } else {
            launch_thresh_start_ms = 0;
        }
        break;
    }

    case ST_BOOST1:
        tvc_update(&att, dt_s);
        if (axial_g < BURNOUT_G && ms_in_state(now) > BURNOUT_DEBOUNCE_MS) {
            tvc_disable();
            enter(ST_COAST1);
        } else if (ms_in_state(now) >= MAX_BURN1_MS) {
            tvc_disable();
            enter(ST_COAST1);   /* backup burnout timeout */
        }
        break;

    case ST_COAST1:
        if (alt.vel_ms <= 0.0f) {
            static int apogee_count = 0;
            if (++apogee_count >= APOGEE_DEBOUNCE_N)
                enter(ST_APOGEE1);
        }
        if (ms_in_state(now) >= APOGEE_TIMEOUT_MS)
            enter(ST_APOGEE1);   /* backup */
        break;

    case ST_APOGEE1:
        enter(ST_STAGE_DELAY);
        break;

    case ST_STAGE_DELAY:
        if (ms_in_state(now) >= STAGE_DELAY_MS && ms_in_state(now) >= MIN_STAGE_DELAY_MS) {
            if (pyro_fire() == 0)
                enter(ST_BOOST2);
            else if (ms_in_state(now) >= STAGE_DELAY_TIMEOUT_MS)
                enter(ST_ABORT);   /* fire failed and we're out of time */
        } else if (ms_in_state(now) >= STAGE_DELAY_TIMEOUT_MS) {
            if (pyro_fire() == 0)
                enter(ST_BOOST2);
            else
                enter(ST_ABORT);
        }
        break;

    case ST_BOOST2:
        /* TVC not active on sustainer in this build - see app_config note */
        if (axial_g < BURNOUT_G && ms_in_state(now) > BURNOUT_DEBOUNCE_MS)
            enter(ST_COAST2);
        else if (ms_in_state(now) >= BOOST2_MAX_MS)
            enter(ST_COAST2);
        break;

    case ST_COAST2:
        if (alt.vel_ms <= 0.0f) {
            static int apogee2_count = 0;
            if (++apogee2_count >= APOGEE_DEBOUNCE_N)
                enter(ST_APOGEE2);
        }
        break;

    case ST_APOGEE2:
        enter(ST_DESCENT);   /* no recovery deployment - free fall from here */
        break;

    case ST_DESCENT:
        if (fabsf(alt.vel_ms) < LAND_VEL_MS) {
            if (ms_in_state(now) >= LAND_DEBOUNCE_MS)
                enter(ST_LANDED);
        } else {
            enter(ST_DESCENT);   /* reset debounce while still moving */
        }
        break;

    case ST_LANDED:
    case ST_ABORT:
    default:
        break;
    }
}

flight_state_e fsm_get_state(void) { return g_fsm.state; }
