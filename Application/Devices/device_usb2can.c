/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)
 * @file 	: device_usb2can.c
 * ******************************************************************************
 * @brief 	: 【introduction】
 * @version	: 1.0
 * @date 	: 2024-08-21
 * @note 	: 【note】
 * *
 * *
 * ******* 1. Improvement *********
 * * 1. Simplify device resource structure members.
 * *
 * * 2. Adds a device state and encapsulates it with an action function
 * *
 * *
 * ******* 2. Fix the bug *********
 * * 1. fix the bug of ...
 * *
 * *
 * ******* 3. TODO *********
 * * 1. waiting to add ...
 * *
 * *
 * ******************************************************************************
 */
/* ---------------------------- user header file ---------------------------- */
#include "device_usb2can.h"

/* ----------------------- global variable definition ----------------------- */

usb2can_device_t g_usb2can_device = {0};

/* ----------------------- public function definition ----------------------- */

/**
 * ******************************************************************************
 * @brief 	: Get a pointer to the device.
 * @retval 	: Pointer to the device structure.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
usb2can_device_t *usb2can_device_get_pointer(void)
{
    return &g_usb2can_device;
}

/* ------------------ private operation function definition ----------------- */

static void usb2can_device_ops_update_action(void *p_object, usb2can_device_action_e action)
{
    usb2can_device_t *p_dev = (usb2can_device_t *)p_object;

    if (action <= USB2CAN_ACTION_UNKNOWN)
    {
        p_dev->params.action = action;
    }
}

static usb2can_device_action_e usb2can_device_ops_return_action(void *p_object)
{
    usb2can_device_t *p_dev = (usb2can_device_t *)p_object;

    return p_dev->params.action;
}

/* --------------------------- Initialize function -------------------------- */

/**
 * ******************************************************************************
 * @brief 	: Register the device with its operations.
 * @param 	: p_object [in/out], Pointer to the device callback function.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
void usb2can_device_register(p_func_callback_dev p_func)
{
    usb2can_device_t *p_dev = &g_usb2can_device;

    // set default parameters

    // set operation function
    p_dev->ops.update_action = usb2can_device_ops_update_action;
    p_dev->ops.return_action = usb2can_device_ops_return_action;

    // bind the callback function
    if (NULL_PTR == p_func || 0 == p_func)
    {
        p_dev->call_func = NULL_PTR;
    }
    else
    {
        p_dev->call_func = p_func;
    }
}
