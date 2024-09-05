/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: device_led.c
 * ******************************************************************************
 * @brief 	: This file implements the led device layer
 * @version	: 2.0
 * @date 	: 2024-08-15
 * @note 	: It encapsulates the functionalities needed to control the LEDs and can be extended for
 * *
 * *
 * ******* 1. Improvement *********
 * * 1. Added members to the device resource structure.
 * *
 * * 2. Change the state-related definition
 * *
 * *
 * *
 * ******* 2. Fix the bug *********
 * * 1. fix the bug of :
 * *
 * *
 * ******************************************************************************
 */
/* ---------------------------- user header file ---------------------------- */
#include "device_led.h"

/* ----------------------- global variable definition ----------------------- */

static const led_device_resources_t g_led_device_resources[LED_ID_COUNT] = {
    {LED_ID_R, GPIO_PIN_RESET, LED_RED_GPIO_Port, LED_RED_Pin},
    {LED_ID_G, GPIO_PIN_RESET, LED_GREEN_GPIO_Port, LED_GREEN_Pin},
};

led_device_t g_led_devices[LED_ID_COUNT] = {0};

/* ----------------------- public function definition ----------------------- */

/**
 * ******************************************************************************
 * @brief 	: get a pointer to an device based on its index.
 * @param 	  index, The index of the led device.
 * @retval 	: This device index corresponds to a device pointer.
 * @author 	: chenningzhan
 * @note	: If the index is out of range, it logs an error and returns NULL.
 * @note	: An open interface for other modules to call to get a device pointer
 * ******************************************************************************
 */
led_device_t *led_device_get_pointer(led_device_index_t index)
{
    if (index < 0 || index >= LED_ID_COUNT)
    {
        return NULL_PTR;
    }
    else
    {
        return &g_led_devices[index];
    }
}

/* ------------------ private operation function definition ----------------- */

/**
 * ******************************************************************************
 * @brief 	: control the status of an led device.
 * @param 	  p_this  	: a pointer to get the device object.
 * @param 	  status  	: The status to set for the led device.
 * @author 	: chenningzhan
 * @note	: sets the led device to on or off or blinking
 * ******************************************************************************
 */
static void led_device_ops_control(void *p_this, led_device_status_t status)
{
    led_device_t *p_dev = (led_device_t *)p_this;
    GPIO_PinState gpio_state;

    switch (status)
    {
    case LED_STATUS_ON:
        HAL_GPIO_WritePin(p_dev->res->gpio_port, p_dev->res->pin, p_dev->res->level_valid);
        break;
    case LED_STATUS_OFF:
        HAL_GPIO_WritePin(p_dev->res->gpio_port, p_dev->res->pin, !p_dev->res->level_valid);
        break;
    case LED_STATUS_BLINKING:
        gpio_state = HAL_GPIO_ReadPin(p_dev->res->gpio_port, p_dev->res->pin);
        HAL_GPIO_WritePin(p_dev->res->gpio_port, p_dev->res->pin, !gpio_state);
        break;
    default:
        break;
    }
}

/**
 * ******************************************************************************
 * @brief 	: register an led device with its associated callback function.
 * @param 	  p_func  	: The callback function pointer for the device operations.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
void led_device_register(p_func_callback_dev p_func)
{
    for (uint8_t i = 0; i < LED_ID_COUNT; i++)
    {
        // gets a pointer to the global variable device
        led_device_t *p_dev = &g_led_devices[i];

        // bind the hardware resource
        p_dev->res = &g_led_device_resources[i];

        // set the default parameter

        // set operation function
        p_dev->ops.control = led_device_ops_control;

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
}
