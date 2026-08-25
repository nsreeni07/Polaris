#pragma once

int  baro_init(void);
int  baro_read(float *alt_m, float *temp_c);
void baro_zero(void);
