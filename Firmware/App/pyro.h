#pragma once
#include <stdint.h>

typedef enum { PYRO_SAFE = 0, PYRO_ARMED, PYRO_FIRED } pyro_state_e;

void pyro_init(void);
int  pyro_arm(void);     /* first latch */
int  pyro_fire(void);    /* second latch + actual fire, re-checks continuity */
int  pyro_continuity_ok(void);
pyro_state_e pyro_get_state(void);
