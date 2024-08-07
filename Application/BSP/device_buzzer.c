/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: device_buzzer.c
 * ******************************************************************************
 * @brief 	: 蜂鸣器设备层实现，向上提供设备ops操作接口
 * @version	: 1.0
 * @date 	: 2024-07-20
 * @note 	: 使用方法：调用 buzzer_device_register 初始化蜂鸣器设备
 *            通过buzzer_device_get_pointer获取设备句柄进行控制
 * ******************************************************************************
 */

/* --------------------------- library header file -------------------------- */
#include <string.h>
/* ---------------------------- user header file ---------------------------- */
#include "device_buzzer.h"

/* ----------------------- global variable definition ----------------------- */
buzzer_device_t g_buzzer_devices = {0};
const buzzer_device_resources_t g_buzzer_device_resources = {
    &htim15, TIM_CHANNEL_2, 150};

/* ----------------------- public function definition ----------------------- */

/**
 * ******************************************************************************
 * @brief 	: buzzer设备开启操作接口
 * @param 	: private_data [in/out], void类型指针，用来获取设备信息
 * @author 	: chenningzhan
 * @note	: 根据蜂鸣器类型（有源或无源），设置GPIO电平或启动PWM信号
 * ******************************************************************************
 */
static void buzzer_device_ops_on(void *private_data)
{
    buzzer_device_t *dev = (buzzer_device_t *)private_data;

    __HAL_TIM_SET_COMPARE(dev->timer, dev->channel, dev->ccr);
    dev->status = BUZZER_STATUS_ON;
}

/**
 * ******************************************************************************
 * @brief 	: buzzer设备关闭操作接口
 * @param 	: private_data [in/out], void类型指针，用来获取设备信息
 * @author 	: chenningzhan
 * @note	: 根据蜂鸣器类型（有源或无源），设置GPIO电平或关闭PWM信号
 * ******************************************************************************
 */
static void buzzer_device_ops_off(void *private_data)
{
    buzzer_device_t *dev = (buzzer_device_t *)private_data;

    __HAL_TIM_SET_COMPARE(dev->timer, dev->channel, 0);
    dev->status = BUZZER_STATUS_OFF;
}

static void buzzer_device_ops_toggle(void *private_data)
{
    buzzer_device_t *dev = (buzzer_device_t *)private_data;

    switch (dev->status)
    {
    case BUZZER_STATUS_ON:
        dev->ops.set_off((void *)dev);
        break;
    case BUZZER_STATUS_OFF:
        dev->ops.set_on((void *)dev);
        break;
    }
}

/**
 * ******************************************************************************
 * @brief 	: 获取buzzer设备句柄
 * @retval 	: 指向设备结构体的指针
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
buzzer_device_t *buzzer_device_get_pointer(void)
{
    return &g_buzzer_devices;
}

/**
 * ******************************************************************************
 * @brief 	: buzzer 设备注册函数，初始化,设置回调函数
 * @param 	: p_func [in/out], 回调函数指针
 * @author 	: chenningzhan
 * @note	: 设置各个buzzer设备的参数以及回调函数
 * ******************************************************************************
 */
void buzzer_device_register(p_dev_call_func p_func)
{
    // dynamic distribution device structure
    buzzer_device_t *dev_source = malloc(sizeof(buzzer_device_t));
    buzzer_device_t *dev_destin = &g_buzzer_devices;

    // set default data
    dev_source->timer = g_buzzer_device_resources.timer;
    dev_source->channel = g_buzzer_device_resources.channel;
    dev_source->ccr = g_buzzer_device_resources.ccr;
    dev_source->status = BUZZER_STATUS_OFF;
    
    // set operation function
    dev_source->ops.set_on = buzzer_device_ops_on;
    dev_source->ops.set_off = buzzer_device_ops_off;
    dev_source->ops.set_toggle = buzzer_device_ops_toggle;
    dev_source->call_func = p_func;
    
    // buzzer timer pwm enable
    HAL_TIM_PWM_Start(dev_source->timer, dev_source->channel);
    // copy data
    memcpy(dev_destin, dev_source, sizeof(buzzer_device_t));
    // Frees dynamically allocated memory
    free(dev_source); 
}
