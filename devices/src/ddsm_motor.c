#include "ddsm_motor.h"
#include "crc.h"
#include "stm32f1xx_hal_uart.h"
#include "uart.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define RAD_TO_POS_SCALE 5215.1895f // 32767 / (2 * PI)
#define POS_TO_RAD_SCALE 0.00019175345f // (2 * PI) / 32767

/**
 * @brief DDSM Motor Initialization
 * 
 * @param uart_instance The UART instance associated with the motor being initialized
 *
 * @note This initialization function relies on DDSM_Motor_Get_ID and DDSM_Motor_Get_Control_Mode to
 * be correctly implemented before initialization. 
*/
DDSM_Motor_Handle_t * DDSM_Motor_Init(UART_Instance_t *uart_instance) {
    DDSM_Motor_Handle_t *motor_handle = (DDSM_Motor_Handle_t *) malloc(sizeof(DDSM_Motor_Handle_t));
    if (motor_handle == NULL) {
        return NULL;
    }

    motor_handle->tx_buffer_len = 10;
    motor_handle->tx_buffer = (uint8_t *) malloc(sizeof(uint8_t) * motor_handle->tx_buffer_len);
    if (motor_handle->tx_buffer == NULL) {
        free(motor_handle);
        return NULL;
    }

    motor_handle->uart_instance = uart_instance;
    motor_handle->speed = 0.0f;
    motor_handle->bus_current = 0.0f;
    motor_handle->position = 0.0f;
    motor_handle->temperature = 0.0f;
    motor_handle->error_code = 0;
    motor_handle->protocol = NONE;
    motor_handle->send_ready = 1;

    // DDSM_Motor_Get_ID(motor_handle);
    // DDSM_Motor_Get_Control_Mode(motor_handle);

    motor_handle->id = 1;
    motor_handle->control_mode = VELOCITY_CONTROL;
    
    return motor_handle;
}

/**
 * @brief Sets motor tx_buffer to send a position command
 *
 * @param motor motor handle
 * @param position desired position in radians
 * @param feedback1 desired feedback for feedback slot 1
 * @param feedback2 desired feedback for feedback slot 2
 *
 * @note This function must be called when the motor is already in position control
*/
HAL_StatusTypeDef DDSM_Motor_Send_Position_Command(DDSM_Motor_Handle_t *motor, float position, DDSM_Motor_Feedback_e feedback1, DDSM_Motor_Feedback_e feedback2) {
    
    // Your implementation here
    
    return DDSM_Motor_Send(motor, motor->uart_instance);
}

/**
 * @brief Sets motor tx_buffer to send a velocity command
 *
 * @param motor motor handle
 * @param position desired velocity in rpm
 * @param feedback1 desired feedback for feedback slot 1
 * @param feedback2 desired feedback for feedback slot 2
 *
 * @note This function must be called when the motor is already in velocity control
*/
HAL_StatusTypeDef DDSM_Motor_Send_Velocity_Command(DDSM_Motor_Handle_t *motor, float velocity, uint8_t acc_time, uint8_t brake, DDSM_Motor_Feedback_e feedback1, DDSM_Motor_Feedback_e feedback2) {
    
    // Your implementation here

    return DDSM_Motor_Send(motor, motor->uart_instance);
}

/**
 * @brief Sets the motor's tx buffer to change the control mode
 *
 * @param motor motor handle
 * @param control_mode desired control mode
*/
HAL_StatusTypeDef DDSM_Motor_Set_Control_Mode(DDSM_Motor_Handle_t *motor, DDSM_Motor_Control_Mode_e control_mode) {
    
    // Your implementation here

    return DDSM_Motor_Send(motor, motor->uart_instance);
}

/**
 * @brief Sets the motor tx buffer to set the motor's id
 *
 * @param motor motor handle
 * @param id desired motor id
*/
HAL_StatusTypeDef DDSM_Motor_Set_ID(DDSM_Motor_Handle_t *motor, uint8_t id) {
    
    // Your implementation here

    return DDSM_Motor_Send(motor, motor->uart_instance);
}

/**
 * @brief Sets the motor tx buffer the get the connected motor's id
 * 
 * @param motor motor handle
 *
 * @note This command does not require a motor ID, allowing us to use it to check the motor ID
 * when intializing.
*/
HAL_StatusTypeDef DDSM_Motor_Get_ID(DDSM_Motor_Handle_t *motor) {
    motor->send_ready = 0;
    motor->protocol = GET_ID;

    motor->tx_buffer[0] = 0xC8;
    motor->tx_buffer[1] = 0x64;
    motor->tx_buffer[2] = 0x00;
    motor->tx_buffer[3] = 0x00;
    motor->tx_buffer[4] = 0x00;
    motor->tx_buffer[5] = 0x00;
    motor->tx_buffer[6] = 0x00;
    motor->tx_buffer[7] = 0x00;
    motor->tx_buffer[8] = 0x00;
    motor->tx_buffer[9] = crc8(motor->tx_buffer, motor->tx_buffer_len - 1);

    motor->send_ready = 1;

    return DDSM_Motor_Send(motor, motor->uart_instance);
}

/**
 * @brief Sets the motor tx buffer to query the motor for its control mode
 *
 * @param motor motor handle
*/
HAL_StatusTypeDef DDSM_Motor_Get_Control_Mode(DDSM_Motor_Handle_t *motor) {
    motor->send_ready = 0;
    motor->protocol = GET_MODE;

    motor->tx_buffer[0] = motor->id;
    motor->tx_buffer[1] = 0x75;
    motor->tx_buffer[2] = 0x00;
    motor->tx_buffer[3] = 0x00;
    motor->tx_buffer[4] = 0x00;
    motor->tx_buffer[5] = 0x00;
    motor->tx_buffer[6] = 0x00;
    motor->tx_buffer[7] = 0x00;
    motor->tx_buffer[8] = 0x00;
    motor->tx_buffer[9] = crc8(motor->tx_buffer, motor->tx_buffer_len - 1);

    motor->send_ready = 1;

    return DDSM_Motor_Send(motor, motor->uart_instance);
}


/**
 * @brief Parses the rx buffer and loads values into the motor struct
 *
 * @param motor motor handle
 *
 * @note This function is called in /Core/src/main.c whenever a receive interrupt is detected 
*/
void DDSM_Motor_Receive(DDSM_Motor_Handle_t *motor) {
    UART_Instance_t *uart_instance = motor->uart_instance;
    if (!crc8_validate(uart_instance->rx_buffer, uart_instance->rx_buffer_len)) {
        return;
    }

    switch (motor->protocol) {
        case SEND_POSITION:

            // Your implementation here

            break;

        case SEND_VELOCITY:
            
            // Your implementation here

            break;

        case GET_ID:
            motor->id = uart_instance->rx_buffer[2];
            break;

        case GET_MODE:
            if (uart_instance->rx_buffer[2] == 0x00) {
                motor->control_mode = OPEN_LOOP;
            } 
            else if (uart_instance->rx_buffer[2] == 0x02) {
                motor->control_mode = VELOCITY_CONTROL;
            } 
            else {
                motor->control_mode = POSITION_CONTROL;
            }
            break;

        case SET_ID:

            // Your implementation here

            break;

        case SET_MODE:
            
            // Your implemenation here

            break;

        case NONE:
            break;

        default:
            break;
    }

}

/**
 * @brief Transmits the tx buffer
 * 
 * @param motor motor handle
 * @param uart_instance uart instance associated with the motor
 *
 * @note This function is called in the hardware interrupt in /bsp/src/uart.c 
*/
HAL_StatusTypeDef DDSM_Motor_Send(DDSM_Motor_Handle_t *motor, UART_Instance_t *uart_instance) {
    HAL_StatusTypeDef status = HAL_ERROR;

    if (uart_instance->transmit_finished && motor->send_ready) {
        uart_instance->transmit_finished = 0;

        memcpy(uart_instance->tx_buffer, motor->tx_buffer, motor->tx_buffer_len);
        motor->send_ready = 0;

        status = HAL_UART_Transmit_IT(uart_instance->uart_handle, uart_instance->tx_buffer, motor->tx_buffer_len);

        if (status != HAL_OK) { // Unlock if failed
            uart_instance->transmit_finished = 1; 
        }
    }
    else {
        status = HAL_BUSY;
    }
    return status;
}

