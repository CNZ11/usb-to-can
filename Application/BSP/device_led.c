/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: device_led.c
 * ******************************************************************************
 * @brief 	: led设备层实现，向上提供led设备ops操作接口
 * @version	: 2.0
 * @date 	: 2024-07-26
 * @note 	: 使用方法：调用 led_device_register 初始化，使用LED设备通过获得操作句柄来控制
 * *
 * *
 * ******* 1. Improvement *********
 * * 1. add device resource structure members
 * *
 * * 2. improvement the function :
 * *
 * *
 * ******* 2. Fix the bug *********
 * * 1. fix the bug of :
 * *
 * *
 * ******* 3. HOW TO USE *********
 * * 0. add the files in this folder to current project
 * * 1. #include 'device_led.h'
 * * 2. change the macro and enum in 'device_led.h':
 * * 3. change the resources in 'device_led.c':
 * * 4. register the device and provides a callback function
 * * 5. Initialize,use a pointer to get an action handle, and use an action function
 * *
 * ******************************************************************************
 */

/* --------------------------- library header file -------------------------- */
#include <string.h>
/* ---------------------------- user header file ---------------------------- */
#include "device_led.h"

/* ----------------------- global variable definition ----------------------- */

led_device_t g_led_devices[LED_ID_COUNT] = {0};
const led_device_resources_t g_led_device_resources[LED_ID_COUNT] = {
    {LED_RED_GPIO_Port, LED_RED_Pin, LED_ID_RED, LED_VALID_LEVEL},
    {LED_GREEN_GPIO_Port, LED_GREEN_Pin, LED_ID_GREEN, LED_VALID_LEVEL},
};

/* ----------------------- public function definition ----------------------- */

/**
 * ******************************************************************************
 * @brief 	: 通过设备id索引获取设备句柄
 * @param 	: index [in/out], 设备索引
 * @retval 	: 指向设备结构体的指针，如果索引无效则返回NULL
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
led_device_t *led_device_get_pointer(led_device_index_t index)
{
    if (index < 0 || index >= LED_ID_COUNT)
    {
        LOG_ERROR("get pointer index out of range");
        return (void *)NULL;
    }
    else
    {
        return &g_led_devices[index];
    }
}

/* ------------------ private operation function definition ----------------- */

static void led_device_ops_update_status(void *private_data, led_device_status_t arg_status)
{
    led_device_t *p_dev = (led_device_t *)private_data;

    if (arg_status <= LED_STATUS_UNKNOWN)
    {
        p_dev->status = arg_status;
    }
    else
    {
        LOG_ERROR("the status unknown");
    }
}

static void led_device_ops_set_on(void *private_data)
{
    led_device_t *p_dev = (led_device_t *)private_data;

    HAL_GPIO_WritePin(p_dev->res.gpio_port, p_dev->res.pin, p_dev->res.valid_level);
    p_dev->ops.update_status((void *)p_dev, LED_STATUS_ON);
}

static void led_device_ops_set_off(void *private_data)
{
    led_device_t *p_dev = (led_device_t *)private_data;

    HAL_GPIO_WritePin(p_dev->res.gpio_port, p_dev->res.pin, !p_dev->res.valid_level);
    p_dev->ops.update_status((void *)p_dev, LED_STATUS_OFF);
}

static void led_device_ops_set_toggle(void *private_data)
{
    led_device_t *p_dev = (led_device_t *)private_data;

    switch (p_dev->status)
    {
    case LED_STATUS_ON:
        p_dev->ops.set_off((void *)p_dev);
        break;
    case LED_STATUS_OFF:
        p_dev->ops.set_on((void *)p_dev);
        break;
    default:
        break;
    }
}

/**
 * ******************************************************************************
 * @brief 	: led 设备注册函数，初始化,设置回调函数
 * @param 	: p_func [in/out], 回调函数指针
 * @author 	: chenningzhan
 * @note	: 设置各个 led 设备的参数以及回调函数
 * ******************************************************************************
 */
void led_device_register(p_dev_call_func p_func)
{
    for (uint8_t i = 0; i < LED_ID_COUNT; i++)
    {
        // dynamic distribution device structure
        led_device_t *dev_source = malloc(sizeof(led_device_t));
        led_device_t *dev_destin = &g_led_devices[i];

        // bind the hardware resource
        dev_source->res.gpio_port = g_led_device_resources[i].gpio_port;
        dev_source->res.pin = g_led_device_resources[i].pin;
        dev_source->res.index = g_led_device_resources[i].index;
        dev_source->res.valid_level = g_led_device_resources[i].valid_level;
        dev_source->status = LED_STATUS_OFF;

        // set operation function
        dev_source->ops.update_status = led_device_ops_update_status;
        dev_source->ops.set_on = led_device_ops_set_on;
        dev_source->ops.set_off = led_device_ops_set_off;
        dev_source->ops.set_toggle = led_device_ops_set_toggle;
        dev_source->call_func = p_func;

        // copy data
        memcpy(dev_destin, dev_source, sizeof(led_device_t));
        // Frees dynamically allocated memory
        free(dev_source);
    }
}
