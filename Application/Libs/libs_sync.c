/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)
 * @file 	: libs_sync.c
 * ******************************************************************************
 * @brief 	: 【introduction】
 * @version	: 1.0
 * @date 	: 2024-08-12
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
 * *
 * ******************************************************************************
 */
/* ---------------------------- user header file ---------------------------- */

#include "libs_sync.h"

/* ----------------------- public function definition ----------------------- */

/**
 * ******************************************************************************
 * @brief 	: 【introduction】
 * @param 	  addr  	: 【parameter】
 * @param 	  event  	: 【parameter】
 * @param 	  time_wait  	: 【parameter】
 * @retval 	: 【return】
 * @author 	: chenningzhan
 * @note 	: 注意数据类型的字节大小要与ram地址上的数据类型对应，否则读取的数据就是错误的
 * ******************************************************************************
 */
int32_t sync_wait_event(uint32_t *addr, uint8_t event, uint32_t time_wait)
{
    volatile uint32_t tick_start, tick_now, tick_wait;
    volatile uint8_t target;

    HAL_GET_TICK(&tick_start); // 获取开始时间
    for (;;)
    {
        HAL_GET_TICK(&tick_now);           // 定期获取当前时间
        tick_wait = tick_now - tick_start; // 计算经过的时间

        target = (*addr);
        if (event == target)
        {
            // 目标事件已发生，返回经过的时间（以毫秒为单位）
            return tick_wait;
        }
        if (tick_wait >= time_wait)
        {
            // 等待超时，返回-1
            return SYNC_TIME_OUT;
        }
    }
}

int32_t sync_wait(uint32_t time_wait)
{
    volatile uint32_t tick_start, tick_now, tick_wait;

    HAL_GET_TICK(&tick_start); // 获取开始时间
    for (;;)
    {
        HAL_GET_TICK(&tick_now);           // 定期获取当前时间
        tick_wait = tick_now - tick_start; // 计算经过的时间

        if (tick_wait >= time_wait)
        {
            // 等待超时，返回-1
            return SYNC_TIME_OUT;
        }
    }
}
