#ifndef __OPS_BYTE_BIT_H__
#define __OPS_BYTE_BIT_H__

/* ---------------------------- user header file ---------------------------- */
#include "./lib.h"

/* ---------------------------- macro definition ---------------------------- */

/**
 * bit operation on a value, such as unsetting, zeroing, or inverting a bit
 */
#define BIT_OPS_SET(x, bit) ((bit) < (sizeof(x) * 8) ? ((x) |= (1 << (bit))) : (x))
#define BIT_OPS_CLR(x, bit) ((bit) < (sizeof(x) * 8) ? ((x) &= ~(1 << (bit))) : (x))
#define BIT_OPS_GET(x, bit) ((bit) < (sizeof(x) * 8) ? ((x) >> (bit) & 1) : 0)

/**
 * size definitions in bytes.
 */
#define SIZE_1Byte 1
#define SIZE_16Byte (16 * SIZE_1Byte)
#define SIZE_32Byte (32 * SIZE_1Byte)
#define SIZE_64Byte (64 * SIZE_1Byte)
#define SIZE_128Byte (128 * SIZE_1Byte)
#define SIZE_256Byte (256 * SIZE_1Byte)
#define SIZE_KB (1024 * SIZE_1Byte)

/* -------------------------- function declaration -------------------------- */

uint8_t reverse_8bits(uint8_t data);

uint32_t combine_bytes_to_uint32_big_endian(uint8_t byte3, uint8_t byte2, uint8_t byte1, uint8_t byte0);
uint32_t combine_bytes_to_uint32_little_endian(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3);

void split_uint32_to_bytes_big_endian(uint32_t value, uint8_t *byte3, uint8_t *byte2, uint8_t *byte1, uint8_t *byte0);
void split_uint32_to_bytes_little_endian(uint32_t value, uint8_t *byte0, uint8_t *byte1, uint8_t *byte2, uint8_t *byte3);

#endif
