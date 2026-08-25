
#include "main.h"
#include "app_main.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_I2C1_Init();
    MX_TIM2_Init();
    MX_SDMMC1_SD_Init();
#if ENABLE_GPS
    MX_USART6_UART_Init();
#endif


    app_init();
    app_loop();   /* never returns */
}
