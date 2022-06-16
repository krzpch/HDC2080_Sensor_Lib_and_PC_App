#include "CppUTestExt/MockSupport_c.h"
#include "usart.h"

UART_HandleTypeDef huart4;

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart,
                                    uint8_t *pData,
                                    uint16_t Size,
                                    uint32_t Timeout)
{
    return (HAL_StatusTypeDef)(mock_c()->actualCall("HAL_UART_Transmit")
                                        ->withMemoryBufferParameter("pData", pData, Size)
                                        ->returnIntValueOrDefault(HAL_OK));
}