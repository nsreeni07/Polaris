/* Booster-only gimbal PID: pitch -> SERVO_PITCH_CH, yaw -> SERVO_YAW_CH.
 * Target is 0 deg (vertical) on both axes. */
#include "tvc_control.h"
#include "app_config.h"
#include "servo.h"

static float s_kp, s_ki, s_kd;
static float s_i_pitch, s_i_yaw;
static float s_prev_pitch_err, s_prev_yaw_err;
static uint8_t s_active;

void tvc_init(void)
{
    s_kp = TVC_KP; s_ki = TVC_KI; s_kd = TVC_KD;
    s_i_pitch = s_i_yaw = 0.0f;
    s_prev_pitch_err = s_prev_yaw_err = 0.0f;
    s_active = 0;
}

void tvc_set_gains(float kp, float ki, float kd) { s_kp = kp; s_ki = ki; s_kd = kd; }

static float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }

static float pid_step(float err, float *integ, float *prev_err, float dt_s)
{
    *integ = clampf(*integ + err * dt_s, -TVC_INTEGRAL_CLAMP_DEG, TVC_INTEGRAL_CLAMP_DEG);
    float deriv = (dt_s > 0.0f) ? (err - *prev_err) / dt_s : 0.0f;
    *prev_err = err;
    float out = s_kp * err + s_ki * (*integ) + s_kd * deriv;
    return clampf(out, -TVC_MAX_DEFLECTION_DEG, TVC_MAX_DEFLECTION_DEG);
}

void tvc_update(const att_state_t *att, float dt_s)
{
    s_active = 1;
    float pitch_out = pid_step(-att->pitch_deg, &s_i_pitch, &s_prev_pitch_err, dt_s);
    float yaw_out   = pid_step(-att->yaw_deg,   &s_i_yaw,   &s_prev_yaw_err,   dt_s);
    servo_set_deg(SERVO_PITCH_CH, pitch_out);
    servo_set_deg(SERVO_YAW_CH, yaw_out);
}

void tvc_disable(void)
{
    s_active = 0;
    servo_set_deg(SERVO_PITCH_CH, 0.0f);
    servo_set_deg(SERVO_YAW_CH, 0.0f);
}
