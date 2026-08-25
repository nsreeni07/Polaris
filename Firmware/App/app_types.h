#pragma once
#include <stdint.h>

typedef struct {
    uint32_t t_ms;
    float ax, ay, az;   
    float gx, gy, gz;  
} imu_sample_t;

typedef struct {
    float alt_m;
    float vel_ms;
} alt_state_t;

typedef struct {
    float pitch_deg;
    float yaw_deg;
    float roll_deg;
} att_state_t;

typedef enum {
    ST_INIT = 0,
    ST_GROUND_IDLE,
    ST_ARMED,
    ST_BOOST1,
    ST_COAST1,
    ST_APOGEE1,
    ST_STAGE_DELAY,
    ST_BOOST2,
    ST_COAST2,
    ST_APOGEE2,
    ST_DESCENT,
    ST_LANDED,
    ST_ABORT,
} flight_state_e;

typedef struct {
    uint32_t t_ms;
    imu_sample_t imu;
    alt_state_t alt;
    att_state_t att;
    uint16_t vbat_mv;
    uint8_t cont_ok;
} fsm_snapshot_t;

typedef struct {
    flight_state_e state;
    fsm_snapshot_t snapshot;
    uint32_t state_entry_ms;
} fsm_t;

extern fsm_t g_fsm;
extern volatile uint8_t g_imu_data_ready;
