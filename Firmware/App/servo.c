/* TIM2, 4-channel PWM. .ioc currently only configures CH1/CH2 - add
 * CH3 (PA2) and CH4 (PA3) there before this will drive all 4 servos. */
#include "servo.h"
#include "app_config.h"
#include "board.h"

static const uint32_t s_channels[SERVO_COUNT] = {
    TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_4
};

int servo_init(void)
{
    for (int i = 0; i < SERVO_COUNT; i++)
        if (HAL_TIM_PWM_Start(&htim2, s_channels[i]) != HAL_OK)
            return -1;
    return 0;
}

void servo_set_us(int ch, uint16_t pulse_us)
{
    if (ch < 0 || ch >= SERVO_COUNT)
        return;
    if (pulse_us < SERVO_MIN_US) pulse_us = SERVO_MIN_US;
    if (pulse_us > SERVO_MAX_US) pulse_us = SERVO_MAX_US;
    __HAL_TIM_SET_COMPARE(&htim2, s_channels[ch], pulse_us);
}

void servo_set_deg(int ch, float deg)
{
    if (deg > 90.0f) deg = 90.0f;
    if (deg < -90.0f) deg = -90.0f;
    servo_set_us(ch, (uint16_t)(SERVO_CENTER_US + deg * SERVO_US_PER_DEG));
}

void servo_center_all(void)
{
    for (int i = 0; i < SERVO_COUNT; i++)
        servo_set_us(i, SERVO_CENTER_US);
}
