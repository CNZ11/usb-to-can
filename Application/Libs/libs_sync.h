#ifndef __LIBS_SYNC_H__
#define __LIBS_SYNC_H__

/* ---------------------------- user header file ---------------------------- */
#include "./user_libs.h"

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

#ifndef NULL
#define NULL ((void *)0)
#endif

/* --------------------------- struct definitions --------------------------- */
typedef enum
{
    SYNC_TIME_OUT = -1,
    SYNC_STATUS_OK = 0,
} sync_status_e;

/* -------------------------- function declaration -------------------------- */

int32_t sync_wait_event(uint32_t *addr, uint8_t event, uint32_t time_wait);

int32_t sync_wait(uint32_t time_wait);

#endif
