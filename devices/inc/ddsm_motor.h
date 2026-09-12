#ifndef DDSM_MOTOR_H
#define DDSM_MOTOR_H


#include "uart.h"
#include <stdint.h>

typedef enum {
    OPEN_LOOP = 0x00,
    VELOCITY_CONTROL = 0x02,
    POSITION_CONTROL = 0x03
} DDSM_Motor_Control_Mode_e;

typedef enum {
    SPEED_FEEDBACK = 0x01,
    BUS_CURRENT_FEEDBACK = 0x02,
    POSITION_FEEDBACK = 0x03
} DDSM_Motor_Feedback_e;

typedef enum {
    SEND_VELOCITY,
    SEND_POSITION,
    SET_MODE,
    SET_ID,
    GET_ID,
    GET_MODE,
    NONE
} DDSM_Motor_Protocol_e;

typedef struct DDSM_Motor {
    UART_Instance_t *uart_instance;

    uint16_t tx_buffer_len;
    uint8_t *tx_buffer;

    uint16_t bus_current; // no need to convert
    float speed; // rpm
    float position; // radians
    uint8_t temperature; // no need to convert
    float acc_time; // ms / rpm
    uint8_t error_code;

    uint8_t id; // motor ID
    DDSM_Motor_Control_Mode_e control_mode; // motor control mode
    DDSM_Motor_Protocol_e protocol; // motor protocol
    DDSM_Motor_Feedback_e feedback1; // useful when receiving position/velocity command
    DDSM_Motor_Feedback_e feedback2; // useful when receiving position/velocity command

    uint8_t send_ready; // flag to know if tx buffer is ready

} DDSM_Motor_Handle_t;

DDSM_Motor_Handle_t * DDSM_Motor_Init(UART_Instance_t *uart_instance);

HAL_StatusTypeDef DDSM_Motor_Send_Position_Command(DDSM_Motor_Handle_t *motor, float position, DDSM_Motor_Feedback_e feedback1, DDSM_Motor_Feedback_e feedback2);

HAL_StatusTypeDef DDSM_Motor_Send_Velocity_Command(DDSM_Motor_Handle_t *motor, float velocity, uint8_t acc_time, uint8_t brake, DDSM_Motor_Feedback_e feedback1, DDSM_Motor_Feedback_e feedback2);

HAL_StatusTypeDef DDSM_Motor_Set_Control_Mode(DDSM_Motor_Handle_t *motor, DDSM_Motor_Control_Mode_e control_mode);

HAL_StatusTypeDef DDSM_Motor_Get_Control_Mode(DDSM_Motor_Handle_t *motor);

HAL_StatusTypeDef DDSM_Motor_Set_ID(DDSM_Motor_Handle_t *motor, uint8_t id);

HAL_StatusTypeDef DDSM_Motor_Get_ID(DDSM_Motor_Handle_t *motor);

void DDSM_Motor_Receive(DDSM_Motor_Handle_t *motor);

HAL_StatusTypeDef DDSM_Motor_Send(DDSM_Motor_Handle_t *motor, UART_Instance_t *uart_instance);

extern DDSM_Motor_Handle_t *ddsm_motor;

#endif /* DDSM210_MOTOR_H */
