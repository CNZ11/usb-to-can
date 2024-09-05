#ifndef __TASK_SCHDULE_H__
#define __TASK_SCHDULE_H__

/* ---------------------------- user header file ---------------------------- */
#include "stm32g4xx_hal.h"

/* ---------------------------- macro definition ---------------------------- */

/**
 * @brief NULL definition safeguard.
 */
#ifndef NULL_PTR
#define NULL_PTR ((void *)0)
#endif

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
 * @brief calculates the number of elements in an array.
 */
#ifndef ARRAY_NUM
#define ARRAY_NUM(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

/**
 * @brief callback function pointer type definition.
 */
typedef void (*p_func_callback_schedule)(void *p_this);

/**
 * @brief callback function pointer type definition.
 */
typedef void (*p_func_normal)(void);

/* --------------------------- struct definitions --------------------------- */

/**
 * @brief Structure for the scheduled task parameters.
 */
typedef struct
{
    volatile uint32_t tick_last;    // The tick count at the time of the last task execution.
    volatile uint32_t tick_current; // The current tick count.
    uint32_t tick_interval;         // The desired interval between task executions.
    uint32_t count;                 // The number of times the task has been executed.
} schedule_params_t;

/**
 * @brief Structure for the scheduled task.
 */
typedef struct
{
    schedule_params_t params;           // Parameters for the scheduled task.
    p_func_callback_schedule call_func; // Pointer to the callback function to be executed.
} schedule_t;

/* -------------------------- function declaration -------------------------- */

void schedule_register(void);

schedule_t *schedule_get_pointer(p_func_callback_schedule p_func);

void schedule_start(void);

#endif