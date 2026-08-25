/* ISR bodies only shown for handlers the App/ layer needs;
 * CubeMX-generated peripheral IRQ handlers otherwise unchanged. */
#include "stm32f7xx_it.h"
#include "app_config.h"
#if ENABLE_GPS
#include "gps_nmea.h"
#endif

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
#if ENABLE_GPS
    if (huart->Instance == USART6) {
        extern UART_HandleTypeDef huart6;
        gps_rx_isr_push((uint8_t)(huart6.Instance->RDR & 0xFF));
    }
#else
    (void)huart;
#endif
}
