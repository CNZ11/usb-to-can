#ifndef __USER_LIBS_H__
#define __USER_LIBS_H__

/* ---------------------------- user header file ---------------------------- */
#include "stm32g4xx_hal.h"
#include "user_log.h"

/* --------------------------- library header file -------------------------- */
#include "stdlib.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* ---------------------------- macro definition ---------------------------- */

/**
 * @brief data type redefinitions for volatile unsigned integers.
 */
typedef volatile unsigned char vuint8_t;
typedef volatile unsigned short vuint16_t;
typedef volatile unsigned int vuint32_t;

/**
 * @brief NULL definition safeguard.
 */
#ifndef NULL_PTR
#define NULL_PTR ((void *)0)
#endif

/**
 * @brief calculates the number of elements in an array.
 */
#ifndef ARRAY_NUM
#define ARRAY_NUM(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

/**
 * @brief https://www.zhaixue.cc/c-arm/c-arm-express.html
 */
#define MAX_TWO(type, x, y) ({ \
    type _x = x;               \
    type _y = y;               \
    _x > _y ? _x : _y;         \
})

typedef enum
{
    ERR_OK = 0,
} error_t;

/**
 * https://www.zhaixue.cc/c-arm/c-arm-express.html
 */

#ifndef UINT8_MAX
#define UINT8_MAX 0xFFU // 255U
#endif

#ifndef UINT16_MAX
#define UINT16_MAX 0xFFFFU // 65535U
#endif

#ifndef UINT32_MAX
#define UINT32_MAX 0xFFFFFFFFU // 4294967295U
#endif

#define UINT16_HALF_MAX (UINT16_MAX >> 1) // 32767
#define UINT32_HALF_MAX (UINT32_MAX >> 1) // 2147483647

/* -------------------------- function declaration -------------------------- */

#endif