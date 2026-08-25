#include "pyro.h"
#include "app_config.h"
#include "board.h"

static pyro_state_e s_state = PYRO_SAFE;

static void fire_pin_low(void) { HAL_GPIO_WritePin(PYRO_FIRE_GPIO_Port, PYRO_FIRE_Pin, GPIO_PIN_RESET); }
static void fire_pin_high(void) { HAL_GPIO_WritePin(PYRO_FIRE_GPIO_Port, PYRO_FIRE_Pin, GPIO_PIN_SET); }

void pyro_init(void)
{
    fire_pin_low();   /* gate safe before anything else runs */
    s_state = PYRO_SAFE;
}

int pyro_continuity_ok(void)
{
    uint32_t mv = 0;
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 5) == HAL_OK)
        mv = (HAL_ADC_GetValue(&hadc1) * 3300) / 4095;
    HAL_ADC_Stop(&hadc1);
    return mv > CONT_THRESH_MV;
}

int pyro_arm(void)
{
    if (s_state != PYRO_SAFE)
        return -1;
    if (!pyro_continuity_ok())
        return -1;
    s_state = PYRO_ARMED;
    return 0;
}

int pyro_fire(void)
{
    if (s_state != PYRO_ARMED)
        return -1;
    if (!pyro_continuity_ok())
        return -1;

    fire_pin_high();
    HAL_Delay(FIRE_PULSE_MS);
    fire_pin_low();

    s_state = PYRO_FIRED;
    return 0;
}

pyro_state_e pyro_get_state(void) { return s_state; }
