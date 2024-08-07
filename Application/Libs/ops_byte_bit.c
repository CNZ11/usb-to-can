/**
 * ******************************************************************************

 * @file 	: ops_byte_bit.c
 * ******************************************************************************
 * @brief 	: 【introduction】
 * @version	: 1.0
 * @date 	: 2024-08-07
 * @note 	: 【note】
 * *
 * *
 * ******* 1. Improvement *********
 * * 1. improvement the function :
 * *
 * * 2. improvement the function :
 * *
 * *
 * ******* 2. Fix the bug *********
 * * 1. fix the bug of :
 * *
 * *
 * ******* 3. HOW TO USE *********
 * * 0. Include this file into your current project.
 * * 1. include the header file where needed.
 * * 2. Modify the macro definitions and enums in 'header_file.h'.
 * * 3. change the resources in 'files_name.c':
 * * 4. Registers the device by calling the provided function using a callback.
 * * 5. Use the pointer to get the operation handle and use the device.
 * *
 * ******************************************************************************
 */

/* ---------------------------- user header file ---------------------------- */
#include "ops_byte_bit.h"

/* ----------------------- public function definition ----------------------- */

/**
 * ******************************************************************************
 * @brief 	: Reverses the bits of a byte
 * @param 	: data [in/out], The byte whose bits are to be reversed.
 * @retval 	: The byte with reversed bits.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
uint8_t reverse_8bits(uint8_t data)
{
    uint8_t reversed = 0;
    for (int i = 0; i < 8; i++)
    {
        reversed <<= 1;
        reversed |= (data & (1 << i)) ? 1 : 0;
    }
    return reversed;
}

/**
 * ******************************************************************************
 * @brief 	: Combines four 8-bit bytes into a 32-bit value using big-endian order.
 * @param 	: byte3 [in], The most significant byte (MSB).
 * @param 	: byte2 [in], The second most significant byte.
 * @param 	: byte1 [in], The third most significant byte.
 * @param 	: byte0 [in], The least significant byte (LSB).
 * @retval 	: The combined 32-bit value
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
uint32_t combine_bytes_to_uint32_big_endian(uint8_t byte3, uint8_t byte2, uint8_t byte1, uint8_t byte0)
{
    uint32_t can_id = 0;

    can_id |= (uint32_t)byte3 << 24;
    can_id |= (uint32_t)byte2 << 16;
    can_id |= (uint32_t)byte1 << 8;
    can_id |= (uint32_t)byte0;

    return can_id;
}

/**
 * ******************************************************************************
 * @brief 	: Combines four 8-bit bytes into a 32-bit value using little-endian order.
 * @param 	: byte0 [in], The least significant byte (LSB).
 * @param 	: byte1 [in], The second least significant byte.
 * @param 	: byte2 [in], The third least significant byte.
 * @param 	: byte3 [in], The most significant byte (MSB).
 * @retval 	: The combined 32-bit value
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
uint32_t combine_bytes_to_uint32_little_endian(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3)
{
    uint32_t can_id = 0;

    can_id |= (uint32_t)byte0;
    can_id |= (uint32_t)byte1 << 8;
    can_id |= (uint32_t)byte2 << 16;
    can_id |= (uint32_t)byte3 << 24;

    return can_id;
}

/**
 * ******************************************************************************
 * @brief 	: Splits a 32-bit value into four 8-bit bytes using big-endian order.
 * @param 	: value [in], The 32-bit value to split.
 * @param 	: byte3 [out], Pointer to store the most significant byte (MSB).
 * @param 	: byte2 [out], Pointer to store the second most significant byte.
 * @param 	: byte1 [out], Pointer to store the third most significant byte.
 * @param 	: byte0 [out], Pointer to store the least significant byte (LSB).
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
void split_uint32_to_bytes_big_endian(uint32_t value, uint8_t *byte3, uint8_t *byte2, uint8_t *byte1, uint8_t *byte0)
{
    *byte3 = (uint8_t)((value >> 24) & 0xFF);
    *byte2 = (uint8_t)((value >> 16) & 0xFF);
    *byte1 = (uint8_t)((value >> 8) & 0xFF);
    *byte0 = (uint8_t)(value & 0xFF);
}

/**
 * ******************************************************************************
 * @brief 	: Splits a 32-bit value into four 8-bit bytes using little-endian order.
 * @param 	: value [in], The 32-bit value to split.
 * @param 	: byte0 [out], Pointer to store the least significant byte (LSB).
 * @param 	: byte1 [out], Pointer to store the second least significant byte.
 * @param 	: byte2 [out], Pointer to store the third least significant byte.
 * @param 	: byte3 [out], Pointer to store the most significant byte (MSB).
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
void split_uint32_to_bytes_little_endian(uint32_t value, uint8_t *byte0, uint8_t *byte1, uint8_t *byte2, uint8_t *byte3)
{
    *byte0 = (uint8_t)(value & 0xFF);
    *byte1 = (uint8_t)((value >> 8) & 0xFF);
    *byte2 = (uint8_t)((value >> 16) & 0xFF);
    *byte3 = (uint8_t)((value >> 24) & 0xFF);
}
