/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: device_buzzer.c
 * ******************************************************************************
 * @brief 	: This file implements the buzzer device layer
 *            providing an operation interface for buzzer devices.
 * @version	: 2.0
 * @date 	: 2024-07-27
 * @note 	: It encapsulates the functionalities needed to control the buzzer and can be extended for
 * *
 * *
 * ******* 1. Improvement *********
 * * 1. Added members to the device resource structure.
 * *
 * * 2. Optimize the code framework.
 * *
 * *
 * ******* 2. Fix the bug *********
 * * 1. fix the bug of :
 * *
 * *
 * ******************************************************************************
 */

/* ---------------------------- user header file ---------------------------- */
#include "device_buzzer.h"

/* ----------------------- global variable definition ----------------------- */

const buzzer_device_resources_t g_buzzer_device_resources = {
    &htim15,
    TIM_CHANNEL_2,
};

buzzer_device_t g_buzzer_devices = {0};

/* ----------------------- public function definition ----------------------- */

/**
 * ******************************************************************************
 * @brief 	: get a pointer to an device
 * @retval 	: Pointer to the buzzer_device_t structure.
 * @author 	: chenningzhan
 * @note	: An open interface for other modules to call to get a device pointer
 * ******************************************************************************
 */
buzzer_device_t *buzzer_device_get_pointer(void)
{
    return &g_buzzer_devices;
}

/* ------------------ private operation function definition ----------------- */

/**
 * ******************************************************************************
 * @brief 	: enable the buzzer by starting the PWM signal.
 * @param 	  p_this  	: a pointer to get the device object.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static void buzzer_device_ops_init(void *p_this)
{
    buzzer_device_t *p_dev = (buzzer_device_t *)p_this;
    HAL_TIM_PWM_Start(p_dev->res->timer, p_dev->res->channel);
}

static void buzzer_device_ops_tick(void *p_this, uint8_t count)
{
    buzzer_device_t *p_dev = (buzzer_device_t *)p_this;
    uint16_t interval_ms = 100;
    uint16_t beep_ms = 10;

    switch (count)
    {
    case 0:
        __HAL_TIM_SET_COMPARE(p_dev->res->timer, p_dev->res->channel, 0);
        break;
    case 1:
        __HAL_TIM_SET_COMPARE(p_dev->res->timer, p_dev->res->channel, p_dev->params.ccr);
        HAL_Delay(beep_ms);
        __HAL_TIM_SET_COMPARE(p_dev->res->timer, p_dev->res->channel, 0);
        break;
    default:
        for (uint8_t i = 0; i < count; i++)
        {
            __HAL_TIM_SET_COMPARE(p_dev->res->timer, p_dev->res->channel, p_dev->params.ccr);
            HAL_Delay(beep_ms);
            __HAL_TIM_SET_COMPARE(p_dev->res->timer, p_dev->res->channel, 0);
            if (i < count - 1)
            {
                HAL_Delay(interval_ms);
            }
        }
        break;
    }
}

/**
 * ******************************************************************************
 * @brief 	: register the buzzer device with its associated callback function.
 * @param 	  p_func  	: The callback function pointer for the device operations.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
void buzzer_device_register(p_func_callback_dev p_func)
{
    // gets a pointer to the global variable device
    buzzer_device_t *p_dev = &g_buzzer_devices;

    // bind the hardware resource
    p_dev->res = &g_buzzer_device_resources;

    // set the default parameter
    p_dev->params.ccr = 150;

    // set operation function
    p_dev->ops.init = buzzer_device_ops_init;
    p_dev->ops.tick = buzzer_device_ops_tick;

    // bind the callback function
    if (NULL_PTR == p_func || 0 == p_func)
    {
        p_dev->call_func = NULL_PTR;
    }
    else
    {
        p_dev->call_func = p_func;
    }

    /* call init operation */
    p_dev->ops.init((void *)p_dev);

    /* call tick operation */
    p_dev->ops.tick((void *)p_dev, 2);
}
