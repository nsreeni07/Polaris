#pragma once
#include <stdint.h>

typedef struct {
    uint8_t  magic;
    uint8_t  state;
    uint32_t t_ms;
    float ax, ay, az;
    float gx, gy, gz;
    float alt_m, vel_ms;
    float pitch_deg, yaw_deg;
    uint16_t vbat_mv;
} log_record_t;

int  datalog_init(void);
void datalog_write(const log_record_t *rec);
void datalog_flush(void);
