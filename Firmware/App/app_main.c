/* no way jose */
#include "board.h"
#include "app_config.h"
#include "app_types.h"
#include "app_main.h"

#include "imu_icm45686.h"
#include "baro_bmp580.h"
#include "servo.h"
#include "pyro.h"
#include "ahrs.h"
#include "kalman.h"
#include "tvc_control.h"
#include "flight_state.h"
#include "datalog.h"
#include "param_store.h"
#include "safety.h"

#if ENABLE_CHARGER
#include "charger_bq25883.h"
#endif
#if ENABLE_GPS
#include "gps_nmea.h"
#endif
#if ENABLE_STATUS_LED
#include "status_led.h"
#endif

static uint32_t s_last_ctrl_ms, s_last_baro_ms, s_last_cont_ms, s_last_flush_ms;
#if ENABLE_CHARGER
static uint32_t s_last_charger_ms;
static uint16_t s_last_vbat_mv;
#endif
#if ENABLE_STATUS_LED
static uint32_t s_last_led_ms;
#endif

void app_init(void)
{
    pyro_init();

#if ENABLE_STATUS_LED
    led_init();
    led_show_state(ST_INIT);
#endif

    servo_init();
    servo_center_all();

    int imu_rc = imu_init();
    int baro_rc = baro_init();
#if ENABLE_CHARGER
    charger_init();
#endif
#if ENABLE_GPS
    gps_init();
#endif

    params_t params;
    param_load(&params);
    tvc_set_gains(params.tvc_kp, params.tvc_ki, params.tvc_kd);

    ahrs_init();
    kalman_init();
    tvc_init();

    if (imu_rc == 0)
        imu_calibrate_gyro();
    if (baro_rc == 0)
        baro_zero();

    int log_rc = datalog_init();
    (void)log_rc;

    safety_init();   /* starts IWDG - call last */

    fsm_init();
#if ENABLE_STATUS_LED
    led_show_state(ST_GROUND_IDLE);
#endif
}

static void run_control_tick(void)
{
    if (!g_imu_data_ready)
        return;

    static uint32_t last_t_ms = 0;
    imu_sample_t s;
    if (imu_read(&s) != 0)
        return;

    float dt_s = (last_t_ms == 0) ? (1.0f / CTRL_HZ) : (float)(s.t_ms - last_t_ms) / 1000.0f;
    last_t_ms = s.t_ms;

    ahrs_update(&s, dt_s);

    float axial_g;
    switch (ARM_UP_AXIS) {
        case 0: axial_g = s.ax; break;
        case 1: axial_g = s.ay; break;
        default: axial_g = s.az; break;
    }
    axial_g *= ARM_UP_SIGN;
    float axial_accel_ms2 = (axial_g - 1.0f) * 9.80665f;
    kalman_predict(axial_accel_ms2, dt_s);

    fsm_tick(&s, dt_s);

    wdg_refresh();
}

void app_loop(void)
{
    for (;;) {
        uint32_t now = HAL_GetTick();

        if ((now - s_last_ctrl_ms) >= (1000u / CTRL_HZ)) {
            s_last_ctrl_ms = now;
            run_control_tick();
        }

        if ((now - s_last_baro_ms) >= (1000u / BARO_HZ)) {
            s_last_baro_ms = now;
            float alt, temp;
            if (baro_read(&alt, &temp) == 0)
                kalman_correct_baro(alt);
        }

        if ((now - s_last_cont_ms) >= (1000u / CONT_HZ)) {
            s_last_cont_ms = now;
            /* ground-status display only; pyro_fire() re-checks continuity itself */
        }

#if ENABLE_CHARGER
        if ((now - s_last_charger_ms) >= (1000u / CHARGER_HZ)) {
            s_last_charger_ms = now;
            charger_read_vbat_mv(&s_last_vbat_mv);
            g_fsm.snapshot.vbat_mv = s_last_vbat_mv;
        }
#endif

#if ENABLE_STATUS_LED
        if ((now - s_last_led_ms) >= (1000u / LED_HZ)) {
            s_last_led_ms = now;
            led_show_state(fsm_get_state());
        }
#endif

        if ((now - s_last_flush_ms) >= LOG_FLUSH_MS) {
            s_last_flush_ms = now;
            datalog_flush();
        }

#if ENABLE_GPS
        gps_poll();
#endif

        static uint32_t last_log_ms = 0;
        if ((now - last_log_ms) >= (1000u / LOG_RATE_HZ)) {
            last_log_ms = now;
            log_record_t rec = {0};
            rec.magic = LOG_RECORD_MAGIC;
            rec.state = (uint8_t)g_fsm.state;
            rec.t_ms = g_fsm.snapshot.t_ms;
            rec.ax = g_fsm.snapshot.imu.ax;
            rec.ay = g_fsm.snapshot.imu.ay;
            rec.az = g_fsm.snapshot.imu.az;
            rec.gx = g_fsm.snapshot.imu.gx;
            rec.gy = g_fsm.snapshot.imu.gy;
            rec.gz = g_fsm.snapshot.imu.gz;
            rec.alt_m = g_fsm.snapshot.alt.alt_m;
            rec.vel_ms = g_fsm.snapshot.alt.vel_ms;
            rec.pitch_deg = g_fsm.snapshot.att.pitch_deg;
            rec.yaw_deg = g_fsm.snapshot.att.yaw_deg;
            rec.vbat_mv = g_fsm.snapshot.vbat_mv;
            datalog_write(&rec);
        }
    }
}
