#include "uart.h"
#include "ddsm_motor.h"
#include <stdint.h>

static UART_Instance_t g_uart1; // statically allocated global uart instance
extern DDSM_Motor_Handle_t *ddsm_motor;

/**
 * @brief Initializes UART instance
 *
 * @param huart UART handle
 * @param rx_buffer pointer to statically allocated rx buffer
 * @param rx_buffer_len rx buffer length
 * @param tx_buffer pointer to statically allocated tx buffer
 * @param tx_buffer_len tx buffer length
 *
 * @note Since this implementation of UART handles receiving and transmission
 * by calling the receive and transmit HAL functions in their interrupts, the
 * UART instance intialization must make an initial call to start up this
 * feedback loop between the interrupt and its peripheral.
*/
UART_Instance_t *UART_Register(UART_HandleTypeDef *huart, uint8_t *rx_buffer, uint16_t rx_buffer_len, uint8_t *tx_buffer, uint16_t tx_buffer_len)
{
    g_uart1.uart_handle = huart;
    g_uart1.rx_buffer = rx_buffer;
    g_uart1.rx_buffer_len = rx_buffer_len;
    g_uart1.tx_buffer = tx_buffer;
    g_uart1.tx_buffer_len = tx_buffer_len;
    g_uart1.receive_finished = 0;
    g_uart1.transmit_finished = 1;

    // Start up receiving interrupt loop
    HAL_UART_Receive_IT(g_uart1.uart_handle,
                        g_uart1.rx_buffer, 
                        g_uart1.rx_buffer_len);

    // Start up transmitting interrupt loop
    HAL_UART_Transmit_IT(g_uart1.uart_handle, g_uart1.tx_buffer, g_uart1.tx_buffer_len);

    return &g_uart1;
}

/**
 * @brief UART receive callback
 * 
 * @param huart UART handle
 * 
 * This function is called whenever a UART peripheral receives
 * rx_buffer_len of bytes. It flips the receive_finished flag to 
 * let the rest of the program know that it can begin to parse the
 * received packet, then begins listening to the peripheral again by 
 * calling HAL_UART_Receive_IT(...).
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        g_uart1.receive_finished = 1;

        HAL_UART_Receive_IT(g_uart1.uart_handle, 
                            g_uart1.rx_buffer,
                            g_uart1.rx_buffer_len);
    }

}

/**
 * @brief UART transmit callback
 *
 * @param huart UART handle
 *
 * This function is called whenever a UART peripheral finishes the
 * transmission of all of its bytes. It flips a flag to let the rest
 * of the program know that it can transmit again, then calls the
 * DDSM_Motor_Send(...) function which handles the transmission logic and
 * calls HAL_UART_Transmit_IT(...).
*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        g_uart1.transmit_finished = 1;
        // DDSM_Motor_Send(ddsm_motor, &g_uart1);
    }
}