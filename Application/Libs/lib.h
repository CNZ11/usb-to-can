/**
 * ******************************************************************************

 * @file 	: lib.h
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
#ifndef __LIB_H__
#define __LIB_H__

/* --------------------------- library header file -------------------------- */
#include "stdlib.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* ---------------------------- user header file ---------------------------- */
#include "stm32g4xx_hal.h"
#include "user_config.h"

/* ---------------------------- macro definition ---------------------------- */

/**
 * data type redefinitions for volatile unsigned integers.
 */
typedef volatile unsigned char vuint8_t;
typedef volatile unsigned short vuint16_t;
typedef volatile unsigned int vuint32_t;

/**
 * NULL definition safeguard.
 */
#ifndef NULL
#define NULL ((void *)(0))
#endif

/**
 * calculates the number of elements in an array.
 */
#define ARRAY_NUM(arr) (sizeof(arr) / sizeof(arr[0]))

/**
 * https://www.zhaixue.cc/c-arm/c-arm-express.html
 */
#define MAX_TWO(type, x, y) ({ \
    type _x = x;               \
    type _y = y;               \
    _x > _y ? _x : _y;         \
})

/* -------------------------- function declaration -------------------------- */

#endif