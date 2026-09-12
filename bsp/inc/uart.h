#ifndef UART_H
#define UART_H

#include "stm32f1xx_hal.h"

#include <stdint.h>

typedef struct _UART_Instance
{
    UART_HandleTypeDef *uart_handle;

    uint8_t *rx_buffer;
    uint16_t rx_buffer_len;

    uint8_t *tx_buffer;
    uint16_t tx_buffer_len;

    volatile uint8_t receive_finished;
    volatile uint8_t transmit_finished;

} UART_Instance_t;

UART_Instance_t *UART_Register(UART_HandleTypeDef *huart, uint8_t *rx_buffer, uint16_t rx_buffer_len, uint8_t *tx_buffer, uint16_t tx_buffer_len);

#endif /* UART_H */