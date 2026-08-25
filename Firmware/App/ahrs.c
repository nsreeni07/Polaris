
#include "ahrs.h"
#include "app_config.h"
#include <math.h>

static att_state_t s_att;
static uint8_t s_launched = 0;

void ahrs_init(void)
{
    s_att.pitch_deg = s_att.yaw_deg = s_att.roll_deg = 0.0f;
    s_launched = 0;
}

void ahrs_notify_launch(void) { s_launched = 1; }

void ahrs_update(const imu_sample_t *s, float dt_s)
{
    s_att.pitch_deg += s->gy * dt_s;
    s_att.yaw_deg   += s->gz * dt_s;
    s_att.roll_deg  += s->gx * dt_s;

    if (!s_launched) {
        float accel_pitch = atan2f(-s->ax, sqrtf(s->ay * s->ay + s->az * s->az)) * 57.2958f;
        float accel_roll  = atan2f(s->ay, s->az) * 57.2958f;
        s_att.pitch_deg = AHRS_GYRO_WEIGHT * s_att.pitch_deg + (1.0f - AHRS_GYRO_WEIGHT) * accel_pitch;
        s_att.roll_deg  = AHRS_GYRO_WEIGHT * s_att.roll_deg  + (1.0f - AHRS_GYRO_WEIGHT) * accel_roll;
    }
}

att_state_t ahrs_get_attitude(void) { return s_att; }
