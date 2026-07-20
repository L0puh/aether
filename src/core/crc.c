#include <aether.h>

#ifdef FEATURE_CRC_APP

static uint16_t crc_table[256];
static uint8_t crc_initialized = 0;

void crc_init(void)
{
    if (crc_initialized) {
        return;
    }

    uint16_t crc;
    for (int i = 0; i < 256; i++) {
        crc = (uint16_t)(i << 8);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (uint16_t)((crc << 1) ^ 0x1021);
            } else {
                crc = (uint16_t)(crc << 1);
            }
        }
        crc_table[i] = crc;
    }
    crc_initialized = 1;
}

uint16_t crc_update(uint16_t crc, uint8_t data)
{
    if (!crc_initialized) {
        crc_init();
    }
    return (uint16_t)((crc << 8) ^ crc_table[(crc >> 8) ^ data]);
}

uint16_t crc_calculate(const uint8_t* data, size_t len, size_t skip_offset, size_t skip_len)
{
    if (!crc_initialized) {
        crc_init();
    }

    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < len; i++) {
        if (i >= skip_offset && i < skip_offset + skip_len) {
            continue;
        }
        crc = crc_update(crc, data[i]);
    }

    return crc;
}


#endif 
