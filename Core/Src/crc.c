#include "crc.h"

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Computes a 1 byte CRC over an array
 *
 * @param data pointer to an array over which the CRC is computed
 * @param length amount of bytes used for CRC
 *
 * @note For this project, compute the CRC over the first 9 bytes
 * of the buffers, then place the result in the last spot in the array.
*/
uint8_t crc8(const uint8_t *data, size_t length) {
    uint8_t crc = 0x00;

    for (size_t i = 0; i < length; i++) {
        crc ^= data[i]; 
        
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x01) {
                crc = (crc >> 1) ^ 0x8C;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return crc;
}

/**
 * @brief Validates a 1 byte CRC over an array
 *
 * @param data pointer to the array to validate
 * @param length amount of bytes to validate
 *
 * @note Validating the CRC requires the computed CRC to be somwhere
 * within the array we are validating. For this project, pass in a 
 * pointer to the buffer and the length of the ENTIRE buffer (i.e. 10).
*/
uint8_t crc8_validate(const uint8_t *data, size_t length) {
    return (crc8(data, length) == 0x00);
}
