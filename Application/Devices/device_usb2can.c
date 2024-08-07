/**
 * ******************************************************************************

 * @file 	: device_usb2can.c
 * ******************************************************************************
 * @brief 	: 【introduction】
 * @version	: 1.0
 * @date 	: 2024-07-24
 * @note 	: 【this file major modifications】
 * *
 * ******* Improvement *********
 * * 1. Simplify device resource structure members.
 * *
 * * 2. Adds a device state and encapsulates it with an action function
 * *
 * ******* Fix the bug *********
 * * 1. fix the bug of function usb2can_device_ops_update_action:
 * *    if is no else
 * *
 * * 2.
 * *
 *
 *
 * ******* HOW TO USE *********
 * * 0. add the files in this folder to current project
 * * 1. #include 'device_usb2can.h'
 * * 2. change the macro and enum in 'device_usb2can.h':
 * * 3. change the resources in 'device_usb2can.c':
 * * 4. Initialize,Use a pointer to get an action handle, and use an action function
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

/**
 * ******************************************************************************
 * @brief 	: Update the action of the device.
 * @param 	  p_self  	: a pointer to get the device object
 * @param 	  action  	: The new action to be set.
 * @author 	: chenningzhan
 * @note 	: None
 * ******************************************************************************
 */
static void usb2can_device_ops_update_action(void *p_self, usb2can_device_action_t action)
{
    usb2can_device_t *p_dev = (usb2can_device_t *)p_self;

    if (action <= USB2CAN_ACTION_UNKNOWN && action >= USB2CAN_ACTION_IDLE)
    {
        p_dev->action = action;
    }
    else
    {
        LOG_ERROR("the action unknown");
    }
}

/**
 * ******************************************************************************
 * @brief 	: Return the current action of the device.
 * @param 	  p_self  	: a pointer to get the device object
 * @retval 	: The current action of the device.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static usb2can_device_action_t usb2can_device_ops_return_action(void *p_self)
{
    usb2can_device_t *p_dev = (usb2can_device_t *)p_self;

    return p_dev->action;
}

/* --------------------------- Initialize function -------------------------- */

/**
 * ******************************************************************************
 * @brief 	: Register the device with its operations.
 * @param 	  p_func  	: The callback function pointer for the device operations.
 * @author 	: chenningzhan
 * @note 	: None
 * ******************************************************************************
 */
void usb2can_device_register(p_callback_func_dev p_func)
{
    // dynamic distribution device structure
    usb2can_device_t *dev_source = malloc(sizeof(usb2can_device_t));
    usb2can_device_t *dev_destin = &g_usb2can_device;

    // set default parameters
    dev_source->action = USB2CAN_ACTION_IDLE;

    // set operation function
    dev_source->ops.update_action = usb2can_device_ops_update_action;
    dev_source->ops.return_action = usb2can_device_ops_return_action;
    if ((void *)NULL != p_func)
    {
        dev_source->call_func = p_func;
    }
    else
    {
        dev_source->call_func = (void *)NULL;
    }

    // copy data
    memcpy(dev_destin, dev_source, sizeof(usb2can_device_t));
    // Frees dynamically allocated memory
    free(dev_source);
}