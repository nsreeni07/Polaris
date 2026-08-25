/* 1-D constant-acceleration Kalman: state = [alt, vel], driven by
 * IMU predict + baro correct, with an outlier gate on baro updates. */
#include "kalman.h"
#include "app_config.h"
#include <math.h>

static float s_alt, s_vel;
static float P[2][2];
static int   s_reject_streak;

void kalman_init(void)
{
    s_alt = 0.0f;
    s_vel = 0.0f;
    P[0][0] = 1.0f; P[0][1] = 0.0f;
    P[1][0] = 0.0f; P[1][1] = 1.0f;
    s_reject_streak = 0;
}

void kalman_predict(float accel_ms2, float dt_s)
{
    s_alt += s_vel * dt_s + 0.5f * accel_ms2 * dt_s * dt_s;
    s_vel += accel_ms2 * dt_s;

    float q = KAL_Q_ACCEL * dt_s;
    P[0][0] += dt_s * (P[1][0] + P[0][1] + dt_s * P[1][1]) + q;
    P[0][1] += dt_s * P[1][1];
    P[1][0] += dt_s * P[1][1];
    P[1][1] += q;
}

void kalman_correct_baro(float alt_m)
{
    float y  = alt_m - s_alt;
    float S  = P[0][0] + KAL_R_BARO;
    float sigma = sqrtf(S);

    if (fabsf(y) > KAL_GATE_SIGMA * sigma) {
        if (++s_reject_streak < KAL_GATE_MAX_REJECT)
            return;                 /* likely outlier, skip this update */
        /* too many rejects in a row - trust the sensor, resync */
    }
    s_reject_streak = 0;

    float K0 = P[0][0] / S;
    float K1 = P[1][0] / S;

    s_alt += K0 * y;
    s_vel += K1 * y;

    float P00 = P[0][0], P01 = P[0][1], P10 = P[1][0], P11 = P[1][1];
    P[0][0] = P00 - K0 * P00;
    P[0][1] = P01 - K0 * P01;
    P[1][0] = P10 - K1 * P00;
    P[1][1] = P11 - K1 * P01;
}

alt_state_t kalman_get_state(void)
{
    alt_state_t s = { s_alt, s_vel };
    return s;
}
