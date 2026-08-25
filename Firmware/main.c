/* CubeMX-generated init calls (MX_GPIO_Init etc.) go here as usual -
 * this file only shows the App/ hookup, not full HAL bring-up. */
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
    /* MX_ADC1_Init() once ADC is added to the .ioc for pyro continuity */
    /* MX_USB_OTG_FS_PCD_Init() once USB is switched to Device_Only */

    app_init();
    app_loop();   /* never returns */
}
