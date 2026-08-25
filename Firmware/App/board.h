#pragma once
#include "stm32f7xx_hal.h"


#define ICM_CS_GPIO_Port    GPIOA
#define ICM_CS_Pin          GPIO_PIN_4   
#define ICM_INT1_GPIO_Port  GPIOC
#define ICM_INT1_Pin        GPIO_PIN_4
#define ICM_INT2_GPIO_Port  GPIOB
#define ICM_INT2_Pin        GPIO_PIN_0

/* SDMMC1 */
#define SD_CS_GPIO_Port     GPIOB
#define SD_CS_Pin           GPIO_PIN_3

/* I2C1 - BQ25883 */
#define BQ_INT_GPIO_Port    GPIOB
#define BQ_INT_Pin          GPIO_PIN_5
#define BQ_CE_GPIO_Port     GPIOB
#define BQ_CE_Pin           GPIO_PIN_8

/* I2C1 - BMP580 */
#define BMP_INT_GPIO_Port   GPIOB
#define BMP_INT_Pin         GPIO_PIN_9

/* Pyro - not yet in .ioc, pick free pins and add there */
#define PYRO_FIRE_GPIO_Port GPIOC
#define PYRO_FIRE_Pin       GPIO_PIN_5
#define PYRO_CONT_ADC_CH    ADC_CHANNEL_8   /* verify against .ioc once ADC is added */

/* Status LED (WS2812B, single data line, bit-banged) */
#define LED_GPIO_Port       GPIOC
#define LED_Pin              GPIO_PIN_6

extern SPI_HandleTypeDef  hspi1;
extern I2C_HandleTypeDef  hi2c1;
extern TIM_HandleTypeDef  htim2;
extern SD_HandleTypeDef   hsd1;
extern UART_HandleTypeDef huart6;   /* GPS, once added to .ioc */
extern ADC_HandleTypeDef  hadc1;    /* pyro continuity, once added to .ioc */
extern IWDG_HandleTypeDef hiwdg;
