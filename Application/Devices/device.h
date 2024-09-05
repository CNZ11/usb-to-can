#ifndef __DEVICE_H__
#define __DEVICE_H__

/* ---------------------------- user header file ---------------------------- */
#include "main.h"
#include "user_config.h"
#include "user_libs.h"

/* --------------------------- library header file -------------------------- */
#include "stdlib.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* ---------------------------- macro definition ---------------------------- */

/**
 * @brief Get the system tick count
 */
#ifndef HAL_GET_TICK
#define HAL_GET_TICK(p_tick)         \
    do                               \
    {                                \
        (*(p_tick)) = HAL_GetTick(); \
    } while (0)
#endif

/**
 * @brief NULL definition safeguard.
 */
#ifndef NULL_PTR
#define NULL_PTR (void *)(0)
#endif

/**
 * @brief calculates the number of elements in an array.
 */
#ifndef ARRAY_NUM
#define ARRAY_NUM(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

/**
 * @brief callback function pointer type definition.
 */
typedef void (*p_func_callback_dev)(void *p_this);

/**
 * @brief callback function pointer type definition.
 */
typedef void (*p_func_register_dev)(p_func_callback_dev p_func);

/* -------------------------- function declaration -------------------------- */

void device_register(void);

#endif