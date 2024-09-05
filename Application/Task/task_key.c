/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: task_key.c
 * ******************************************************************************
 * @brief 	: This file implements the key handling task.
 *            It provides an interface for managing key presses and associated actions.
 * @version	: 1.0
 * @date 	: 2024-07-27
 * @note 	: 【note】
 * *
 * *
 * ******* 1. Improvement *********
 * * 1. Improved the functionality of the key handling task.
 * *
 * * 2. improvement the function.
 * *
 * *
 * ******* 2. Fix the bug *********
 * * 1. fix the bug of :
 * *
 * *
 * ******* 3. HOW TO USE *********
 * * 0. Include this file into your current project.
 * * 1. include the header file where needed.
 * * 2. Modify the macro definitions and enums in 'header file.h'.
 * * 3. change the resources in 'files name.c':
 * * 4. Registers the device by calling the provided function using a callback.
 * * 5. Use the pointer to get the operation handle and use the device.
 * *
 * ******************************************************************************
 */

/* ---------------------------- user header file ---------------------------- */
#include "task_key.h"
#include "device_buzzer.h"
#include "device_key.h"

/* --------------------------- function definition -------------------------- */

/**
 * ******************************************************************************
 * @brief 	: Callback function for key device operations.
 * @param 	  p_self  	: Pointer to the device structure.
 * @author 	: chenningzhan
 * @note	: called when a key event occurs. It handles the logging
 *            and interaction with other devices, such as the buzzer.
 * ******************************************************************************
 */
static void key_device_ops_callback(void *p_self)
{
    // get the device operate pointer
    key_device_t *p_dev_key = (key_device_t *)p_self;
    buzzer_device_t *p_dev_buzzer = buzzer_device_get_pointer();

    // Log key event
    switch (p_dev_key->res->index)
    {
    case KEY_ID_CTRL:
        LOG_DEBUG("KEY_ID_CTRL!");
        break;
    default:
        break;
    }

    // trigger buzzer action
    p_dev_buzzer->ops.tick((void *)p_dev_buzzer, 1);
}

/**
 * ******************************************************************************
 * @brief 	: Entry function for the key handling task.
 * @param 	  p_self  	: a pointer to get the task object
 * @author 	: chenningzhan
 * @note	: enters a loop to continuously check the status of the key device.
 * ******************************************************************************
 */
void mytask_key_entry(void *p_self)
{
    schedule_t *task = (schedule_t *)p_self;
    static key_device_t *p_dev_ctrl = NULL_PTR;

    /* the task initialization */
    if (0 == task->params.count)
    {
        key_device_register(key_device_ops_callback);
        p_dev_ctrl = key_device_get_pointer(KEY_ID_CTRL);
    }
    /* the task starts a loop */
    else
    {
        // check the status of each keystroke device
        p_dev_ctrl->ops.update_status((void *)p_dev_ctrl);
    }
}
