#ifndef __LIBS_CALC_H__
#define __LIBS_CALC_H__

/* ---------------------------- user header file ---------------------------- */
#include "./user_libs.h"

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

/**
 * @brief Calculates the minimum integer quotient when dividing the dividend by the divisor.
 * @param dividend : The dividend to be divided (uint32_t).
 * @param divisor : The divisor by which the dividend is to be divided (uint32_t), must be greater than zero.
 * @note This macro assumes that both dividend and divisor are greater than zero.
 */
#define CALC_DIVIDE_INTEGER_MIN(dividend, divisor) \
    ((dividend) / (divisor) + (((dividend) % (divisor)) ? 1 : 0))

/* -------------------------- function declaration -------------------------- */

uint32_t calc_divide_integer_min(uint32_t dividend, uint32_t divisor);

uint8_t reverse_8bits(uint8_t data);

uint32_t combine_bytes_to_uint32_big_endian(uint8_t byte3, uint8_t byte2, uint8_t byte1, uint8_t byte0);
uint32_t combine_bytes_to_uint32_little_endian(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3);

void split_uint32_to_bytes_big_endian(uint32_t value, uint8_t *byte3, uint8_t *byte2, uint8_t *byte1, uint8_t *byte0);
void split_uint32_to_bytes_little_endian(uint32_t value, uint8_t *byte0, uint8_t *byte1, uint8_t *byte2, uint8_t *byte3);

#endif
