# Onboarding Project 2 - Motor Driver

## Overview

The goal of this project is to write firmware which is capable of driving, switching control modes, and reading information from a DDSM210 motor using an STM32 Bluepill (STM32F103C6). You will be communicating with the motor using a UART (Universal Asynchronous Receiver Transmitter) protocol, and this project will guide you through the process.

## Background Info

### UART

Universal Asynchronous Receiver Transmitter (UART) is a serial communication protocol that allows for the exchange of data between two devices. At its most basic level, UART transmits a series of bits, where each bit is either a 0 or a 1. For example, the transmitter might use a high voltage to represent a 1 and a low voltage to represent a 0. The UART protocol is a standard
agreement between devices that determines what different sequences of setting the wire to high and low represents.

Unlike other communication protocols, UART is asynchronous, meaning that there is no wire connecting the two devices which has a shared clock signal. Instead, both devices agree ahead of time on a rate of transmission, otherwise known as a **baud rate**. The receiver uses this agreed-upon timing to determine when it should sample the incoming signal. The baud rate the DDSM210 motor requires is 115200.

The UART port on the STM32 Bluepill has two pins, labeled RX (receive) and TX (transmit). In order to establish communication between two devices, the TX pin from one device must be connected to the other device's RX pin. The two devices must also share a common ground, which is sometimes seen as a third wire between two devices. Becasue the transmit and receive lines are separate, UART supports full-duplex communication, meaning you can receive and transmit at the same time.

UART transmits data through individual frames. A typical frame contains a start bit, the number of bits, a parity bit, then one or more stop bits. The line sits idle (high) before transmission begins. The transmitter changes the line to indicate the start of a frame, sends the data bits, parity bit for basic error detection, and then returns the line to the idle state with the stop bit. The DDSM210 motor communicates with 8 data bits, 1 stop bit, no parity, and a data length of 10 bytes.

An important note is that UART is a protocol which is implemented by the hardware, and the STM32 and DDSM210 motor have UART peripherals built into them. Your software configures that peripheral with settings such as the baud rate, number of data bits, parity, the number of stop bits, and loads the data. After configuration, the peripheral handles the actual timing and conversion between bytes in memory and electrical signals on the TX/RX pins.

For this project, the UART peripheral has already been configured. In orderer to send a packet you load the command you want to send into an array, then call a function to send a transmit command to the motor with that array. This interface will be described in more detail later. To see the configuration, you can look [here](./Core/Src/main.c#L175) in the `MX_USART1_UART_Init(...)` function.

### Hardware Interrupts

A hardware interrupt is a mechanism by which an external device or peripheral can get the processor's attention whenever something important happens. Instead of the CPU constantly checking every device to see if it needs attention, the hardware can send an interrupt request to the CPU, after which the CPU pauses what it is doing, handles the event, then continues where it left off.

One situation where hardware interrupts are very useful is when receiving and transmitting data over UART. Since the hardware takes care of receiving and transmitting (independently fromm the CPU), we do not want to waste CPU cycles checking if we have received new information or if our transmit has finished, so we use interrupts. The STM32 Bluepill is configured to fire an interrupt whenever a full frame has been received, and whenever a transmit has finished.

In practice, an interrupt is a function that the processor jumps to whenever it receives an interrupt signal, either from a physical interrupt pin or internally from a peripheral. For UART, we implement two functions: `HAL_UART_RxCpltCallback()` and `HAL_UART_TxCpltCallback()`, which the code executes whenever a receive and transmit have finished respectively (implemented [here](./bsp/Src/uart.c)). In the interrupts we flip a flag in our global UART struct, letting the processor know that they can begin either processing the received data or send the next command.

### CRC (Cyclic Redundancy Checks)

As data travers wires from device to device, electromagnetic interference (among other things) can cause fluctuations in the sent signal, leading to the corruption of data. Malformed data can have unforseen circumstances, so before you parse a received packet you must verify that it isn't corrupted. The most common way to verify that your packet is correct is using a cyclic redundancy check (CRC), otherwise known as a checksum. The checksum is computed by performing a mathematic operation on all the bytes that were sent and is stored usually as the last byte in the packet. Then, the receiving device can recompute the checksum using that same mathematical operation to see if it matches, verifying that the packet was not corrupted.

In this project, the DDSM210 uses the CRC-8 maxim checksum algorithm, which has already been implemented. However, you will need to call the `crc8(...)` and `crc8_validate(...)` function located [here](./Core/Src/crc.c) to compute and validate checksums respectively.

## Prerequisites

- A laptop, (either MacOS or Windows) with a usb port.
- Basic knowledge of C
- Completed [Introduction to Embedded 1](https://github.com/RoboMaster-Club/Onboarding-Project-1)

## [!] Start Here

### Part 1: Setup and Clone Repository

- In order to complete this project you will need all of the [tools used in the last project](https://github.com/RoboMaster-Club/Onboarding-Project-1?tab=readme-ov-file#part-1-download-tools).

- Clone the repository by pasting the following command in your terminal:
```
git clone https://github.com/PurdueRM/Onboarding-Project-2.git
```

### Part 2: Familiarize yourself with the repository

There are three important folders in this repository, with which you will need to familiarize yourself in order to complete this project:
-  **Drivers**: This folder contains the HAL (Hardware Abstraction Layer) and CMSIS (Cortex Microcontroller Software Interface Standard) folder which are generated by STM32 and act as the lowest-level software level that allows you to communicate with the STM32 hardware. Do not touch this folder.
- **bsp**: This folder, which stands for board support package, contains the user written drivers for different peripherals, and define the hardware interrupts that the peripheral uses. In this repository, we only use UART, so that is the only file found there.
- **Core**: This folder is the core of the project. There are several files you should be aware of:
    - **main.c:** This is the file that contains the main function of the program, which handles all of the initialization of the hardware. The function serves as an entry point into the program, and is where your main control loop will go.
    - **crc.c:** This is the file which implements a checksum algorithm that you will need to use.
    - The rest of the files in this folder handle other declarations, such as for system calls, interrupts, and memory handling. Do not change anything in these files.
- **devices**: This folder is were you will be doing the bulk of you work, and is where a template for a DDSM210 motor driver that you will need to finish implementing is located. 

### Part 3: Writing the motor driver

Now, you will be writing a driver for the DDSM210 motor by following its documentation. The wiki for the motor is located here:

### https://www.waveshare.com/wiki/DDSM210

There are 6 different commands that you will need to send to the motor, which will be sent using the following functions:
- `DDSM_Motor_Send_Position_Command(...)`
- `DDSM_Motor_Send_Velocity_Command(...)`
- `DDSM_Motor_Set_Control_Mode(...)`
- `DDSM_Motor_Get_Control_Mode(...)`
- `DDSM_Motor_Set_ID(...)`
- `DDSM_Motor_Get_ID(...)`

You will also have to parse the incoming data from the motor in this function:
- `DDSM_Motor_Receive(...)`

All these functions are located in [this](./devices/src/ddsm_motor.c) file. In order to complete this project, you will need to implement all of these functions and demostrate that they work, either through debugging and watching global structs or by physically moving the motor.

One example of how to parse incoming data, and one example on how to send a command to the motor will be given to you, but feel free to have your own implementation.

When receiving information, the goal is to fill this DDSM motor struct with all of the valid information:
```
typedef struct DDSM_Motor {
    UART_Instance_t *uart_instance;

    uint16_t tx_buffer_len;
    uint8_t *tx_buffer;

    uint16_t bus_current;
    float speed; // rpm
    float position; // radians
    uint8_t temperature;
    float acc_time; // ms / rpm
    uint8_t error_code;

    uint8_t id;
    uint8_t control_mode;
    uint8_t protocol;
    uint8_t send_ready;
    DDSM_Motor_Feedback_e feedback1;
    DDSM_Motor_Feedback_e feedback2;

} DDSM_Motor_Handle_t;
```

An important characteristic about the DDSM210 motor is that it sends information to the STM32 BluePill only after it has received a command, and the format of the response is determined by the command that was sent. Thus, it is important to set the protocol, control_mode, and feedbacks correctly so when the STM32 microcontroller receives the command's feedback it knows how to parse it.

### Testing

We will have three DDSM210 motors with STM32 Bluepills for you to test your code with. Feel free to come to any meeting or office hours to test. You will use the same procedure as the previous project. There is an intial list of commands that we will be testing in the main function, but feel free to change those during debugging.