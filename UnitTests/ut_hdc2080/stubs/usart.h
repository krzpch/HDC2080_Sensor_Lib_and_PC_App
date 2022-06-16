#ifndef __USART_H__
#define __USART_H__

#include <stdint.h>

typedef struct 
{
    uint8_t dummy;
} UART_HandleTypeDef;

typedef enum 
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

extern UART_HandleTypeDef huart4;

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart,
                                    uint8_t *pData,
                                    uint16_t Size,
                                    uint32_t Timeout);

#endif /* __USART_H__ */