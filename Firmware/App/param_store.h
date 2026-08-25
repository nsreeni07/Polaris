#pragma once
#include <stdint.h>

typedef struct {
    float tvc_kp, tvc_ki, tvc_kd;
    float baro_ground_pa;
    uint32_t crc;
} params_t;

void param_load(params_t *out);   /* falls back to defaults on any fault */
int  param_save(const params_t *in);
