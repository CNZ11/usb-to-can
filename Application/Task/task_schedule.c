/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: task_schedule.c
 * ******************************************************************************
 * @brief 	: use systick for easy task scheduling
 * @version	: 4.0
 * @date 	: 2024-07-27
 * @note 	: improved access to system time base for ease of migration and maintenance
 * *
 * ******* Improvement *********
 * * 1. the system timebase acquisition method is encapsulated into a function
 * *    and the interface is called in other places which is more convenient for portability:
 * *
 * * 2. added the task structure pointer to obtain the task structure pointer through the task entry function pointer
 * *    and you can dynamically modify the task parameters
 * *
 * * 3. Encapsulating functions, changing code frames, variable naming styles
 * *
 * *
 * ******* Fix the bug *********
 * * 1. fix the bug of :
 * *
 * *
 * ******* HOW TO USE *********
 * * 0. add the files in this folder to current project
 * * 1. #include 'task_schedule.h'
 * * 2. change the macro in TASK_MAX_COUNT:
 * * 3. use the function 'schedule_register' to register tasks to the schedule list
 * * 4. call the function 'schedule_start' in the loop
 * *
 * ******************************************************************************
 */

/* ---------------------------- user header file ---------------------------- */

#include "task_schedule.h"
#include "task_key.h"
#include "task_led.h"
#include "task_usb2can.h"

/* ----------------------- global variable definition ----------------------- */

/**
 * @brief this is normal task,only init device.
 */
p_func_normal g_init_list[] = {
    mytask_usb2can_entry,
};

/**
 * @brief this is multiple running task
 */
schedule_t g_task_list[] = {
    {
        .call_func = mytask_led_entry,
        .params.tick_interval = 200,
    },
#if 0
    {
        .call_func = mytask_key_entry,
        .params.tick_interval = 30,
    },
#endif
};

/* ----------------------- public function definition ----------------------- */

/**
 * ******************************************************************************
 * @brief 	: Retrieve a pointer to a scheduled task using its callback function pointer.
 * @param 	  p_func  	: func The task function pointer.
 * @retval 	: Pointer to the task structure or NULL if not found.
 * @author 	: chenningzhan
 * @note 	: None
 * ******************************************************************************
 */
schedule_t *schedule_get_pointer(p_func_callback_schedule p_func)
{
    if (NULL_PTR == p_func || 0 == p_func)
        return NULL_PTR;

    for (int i = 0; i < ARRAY_NUM(g_task_list); i++)
    {
        if (p_func == g_task_list[i].call_func)
        {
            return &g_task_list[i];
        }
    }
    return NULL_PTR;
}

void schedule_register(void)
{
    // The task callback function is executed once first,to init the some parameters
    for (int i = 0; i < ARRAY_NUM(g_task_list); i++)
    {
        g_task_list[i].call_func((void *)&g_task_list[i]);
        g_task_list[i].params.count += 1;
    }

    // init the some parameters
    for (int i = 0; i < ARRAY_NUM(g_init_list); i++)
    {
        g_init_list[i]();
    }
}

/**
 * ******************************************************************************
 * @brief 	: Start the execution of all registered tasks according to their intervals.
 * @author 	: chenningzhan
 * @note	: This function checks each task's initialization status and runs them based on
 *            their tick intervals. It updates the current tick and the last tick, and calls the task's
 *            callback function if the interval condition is met.
 * ******************************************************************************
 */
void schedule_start(void)
{
    for (int i = 0; i < ARRAY_NUM(g_task_list); i++)
    {
        // update the current tick
        HAL_GET_TICK(&g_task_list[i].params.tick_current);

        // task running condition check
        if ((g_task_list[i].params.tick_current - g_task_list[i].params.tick_last) >= g_task_list[i].params.tick_interval)
        {
            g_task_list[i].call_func((void *)&g_task_list[i]);
            HAL_GET_TICK(&g_task_list[i].params.tick_last);
            g_task_list[i].params.count += 1;
        }
    }
}
