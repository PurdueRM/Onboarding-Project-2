#ifndef CRC_H
#define CRC_H

#include <stdint.h>
#include <stddef.h>

uint8_t crc8(const uint8_t *data, size_t length);

uint8_t crc8_validate(const uint8_t *data, size_t length);

#endif /* CRC_H */
