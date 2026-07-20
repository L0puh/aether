#ifndef CRC_H
#define CRC_H

#include <defs.h>

#ifdef FEATURE_CRC_APP

void crc_init(void);
uint16_t crc_update(uint16_t crc, uint8_t data);
uint16_t crc_calculate(const uint8_t* data, size_t len, size_t skip_offset, size_t skip_len);
#endif 

#endif 
