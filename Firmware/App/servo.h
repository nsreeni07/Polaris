#pragma once
#include <stdint.h>

#define SERVO_COUNT 4

int  servo_init(void);
void servo_center_all(void);
void servo_set_us(int ch, uint16_t pulse_us);
void servo_set_deg(int ch, float deg);   /* deg relative to center, +/-90 */
