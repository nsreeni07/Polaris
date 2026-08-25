#pragma once
#include <stdint.h>

typedef struct {
    float lat, lon;
    float alt_m;
    uint8_t fix;
} gps_fix_t;

void gps_init(void);
void gps_poll(void);        /* call every loop pass, cheap */
gps_fix_t gps_get_fix(void);
