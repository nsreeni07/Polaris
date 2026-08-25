#include "safety.h"
#include "app_config.h"
#include "board.h"
#include <math.h>

void safety_init(void)
{
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
    hiwdg.Init.Reload = 1250;   /* ~1s timeout at 32kHz LSI / 32 */
    HAL_IWDG_Init(&hiwdg);
}

void wdg_refresh(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}

int safety_check_arm_gate(const fsm_snapshot_t *snap)
{
    float lateral_g = sqrtf(snap->imu.ax * snap->imu.ax + snap->imu.ay * snap->imu.ay);
    float tilt_g = fabsf(snap->imu.az - 1.0f);

    if (fabsf(snap->alt.vel_ms) > ARM_MAX_VEL_MS) return 0;
    if (tilt_g > ARM_MAX_TILT_G) return 0;
    if (lateral_g > ARM_MAX_LATERAL_G) return 0;
    if (snap->vbat_mv < ARM_MIN_VBAT_MV) return 0;   /* also fails safe if ENABLE_CHARGER=0 */
    if (!snap->cont_ok) return 0;

    return 1;
}
