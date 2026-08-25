#pragma once

/* ----random shi--- */
#define ENABLE_CHARGER            0
#define ENABLE_GPS                0
#define ENABLE_STATUS_LED         0

/* ---- shedule in my calender---- */
#define CTRL_HZ                200
#define BARO_HZ                 50
#define CONT_HZ                 10
#define CHARGER_HZ               1
#define LED_HZ                   5
#define LOG_FLUSH_MS            500

/* ---- Flight detection ---- */
#define LAUNCH_G                3.0f
#define LAUNCH_DEBOUNCE_MS      150
#define LAUNCH_BARO_ALT_M       10.0f
#define BURNOUT_G                0.5f
#define BURNOUT_DEBOUNCE_MS     200
#define MAX_BURN1_MS            6000
#define APOGEE_DEBOUNCE_N        10
#define APOGEE_TIMEOUT_MS      30000

/* ---- Stage 2 boom? ---- */
#define STAGE_DELAY_MS            500
#define STAGE_DELAY_TIMEOUT_MS   3000
#define MIN_STAGE_DELAY_MS        200
#define BOOST2_MAX_MS            4000

/* ---- Descent / mission end(Or Big boom) ---- */
#define LAND_VEL_MS                    1.0f
#define LAND_DEBOUNCE_MS              10000

/* ---- fire🔥 ---- */
#define FIRE_PULSE_MS            1000
#define FIRE_RETRY_MAX               1
#define CONT_THRESH_MV             300
#define TEST_FIRE_PASSCODE       0x52A5u

/* ---- TVC control  ---- */
#define TVC_KP                    0.8f
#define TVC_KI                    0.05f
#define TVC_KD                    0.15f
#define TVC_MAX_DEFLECTION_DEG    8.0f
#define TVC_INTEGRAL_CLAMP_DEG    3.0f
#define SERVO_CENTER_US           1500u
#define SERVO_MIN_US               500u
#define SERVO_MAX_US              2500u
#define SERVO_US_PER_DEG           ((SERVO_MAX_US - SERVO_MIN_US) / 180.0f)
#define SERVO_PITCH_CH                0
#define SERVO_YAW_CH                  1

/* ---- Sensors ---- */
#define ACCEL_FS_G                16
#define ACCEL_SAT_G                15.5f
#define GYRO_FS_DPS              2000
#define GYRO_CAL_SAMPLES          400
#define GYRO_CAL_MAX_PP_DPS         5.0f
#define GYRO_CAL_ACC_BAND_G         0.15f
#define SEA_LEVEL_PA_DEFAULT   101325.0f

/* ---- AHRS ---- */
#define AHRS_GYRO_WEIGHT             0.98f

/* ---- Kalman ---- */
#define KAL_Q_ACCEL                4.0f
#define KAL_R_BARO                 1.5f
#define KAL_GATE_SIGMA              5.0f
#define KAL_GATE_MAX_REJECT          25

/* ---- Logging ---- */
#define LOG_RECORD_MAGIC          0xA5
#define LOG_RATE_HZ                200
#define LOG_RING_BYTES            65536
#define LOG_FILENAME              "FLIGHT.BIN"

/* ---- Arming gate ---- */
#define ARM_MAX_VEL_MS               2.0f
#define ARM_MAX_TILT_G                0.5f
#define ARM_MAX_LATERAL_G             0.35f
#define ARM_MIN_VBAT_MV            7000
#define ARM_UP_AXIS                     2
#define ARM_UP_SIGN                     1.0f
