/* WS2812B, single data line, bit-banged (blocking). Compiled out
 * unless ENABLE_STATUS_LED=1. Timing is approximate - scope it once
 * on hardware; a PWM+DMA approach is more reliable if colors glitch. */
#include "status_led.h"
#include "board.h"

static void send_bit(uint8_t bit)
{
    if (bit) {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        for (volatile int i = 0; i < 14; i++) __NOP();
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        for (volatile int i = 0; i < 6; i++) __NOP();
    } else {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        for (volatile int i = 0; i < 6; i++) __NOP();
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        for (volatile int i = 0; i < 14; i++) __NOP();
    }
}

static void send_byte(uint8_t b)
{
    for (int i = 7; i >= 0; i--)
        send_bit((b >> i) & 1);
}

static void set_color(uint8_t r, uint8_t g, uint8_t b)
{
    send_byte(g);
    send_byte(r);
    send_byte(b);
}

void led_init(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

void led_show_state(flight_state_e state)
{
    switch (state) {
        case ST_INIT:         set_color(20, 20, 20); break;
        case ST_GROUND_IDLE:  set_color(0, 0, 40); break;
        case ST_ARMED:        set_color(40, 0, 0); break;
        case ST_BOOST1:
        case ST_BOOST2:       set_color(40, 40, 0); break;
        case ST_ABORT:        set_color(40, 0, 40); break;
        default:              set_color(0, 40, 0); break;
    }
}
