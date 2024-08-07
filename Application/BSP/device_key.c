/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: device_key.c
 * ******************************************************************************
 * @brief 	: the key device layer provides the ops operation interface of the key device
 * @version	: 2.0
 * @date 	: 2024-07-15
 * @note 	: 按键没有长按处理，带有单击触发间隔功能（使用systick时基实现）
 *            如需同步等待按键事件，将待处理事件在回调函数里面使用
 *            如需主动查询按键状态，通过调用 key_device_ask_status 函数即可
 * ******************************************************************************
 */

/* --------------------------- library header file -------------------------- */
#include <string.h>

/* ---------------------------- user header file ---------------------------- */
#include "device_key.h"

/* ----------------------- global variable definition ----------------------- */

key_device_t g_key_devices[KEY_DEVICE_COUNT] = {0};
const key_device_resources_t g_key_device_resources[KEY_DEVICE_COUNT] = {
    {KEY_CTRL_GPIO_Port, KEY_CTRL_Pin, KEY_ID_CTRL},
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
key_device_t *key_device_get_pointer(key_device_index_t index)
{
    if (index < 0 || index >= KEY_DEVICE_COUNT)
    {
        LOG_ERROR("get pointer index out of range");
        return NULL;
    }
    else
    {
        return &g_key_devices[index];
    }
}

/**
 * ******************************************************************************
 * @brief 	: 按键查询状态，并返回按下的按键的ID
 * @retval 	: 有按下按键返回其ID，否则返回-1
 * @author 	: chenningzhan
 * @note	: 供其他文件调用，查询每个按键状态
 * ******************************************************************************
 */
key_device_index_t key_device_return_status(void)
{
    key_device_t *p_key_dev_ctrl = key_device_get_pointer(KEY_ID_CTRL);

    // if key is pressed,return the key index
    if (KEY_STATUS_PRESS == p_key_dev_ctrl->status)
        return p_key_dev_ctrl->index;
    // if no key is pressed,return -1
    else
        return -1;
}

/**
 * ******************************************************************************
 * @brief 	: 设置下一次按键按下的时间
 * @param 	: private_data [in/out], void类型指针，用来获取设备信息
 * @author 	: chenningzhan
 * @note	: 通过systick时基设置时间间隔
 * ******************************************************************************
 */
static void key_device_ops_update_time_next(void *private_data)
{
    key_device_t *dev = (key_device_t *)private_data;

    uint32_t time_current = HAL_GetTick();
    dev->time_next = time_current + dev->time_interval;
}

/**
 * ******************************************************************************
 * @brief 	: 通过主动查询方式读取按键电平
 * @param 	: private_data [in/out], void类型指针，用来获取设备信息
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static void key_device_ops_update_level(void *private_data)
{
    key_device_t *dev = (key_device_t *)private_data;

    dev->level_current = HAL_GPIO_ReadPin(dev->gpio_port, dev->pin);
}

/**
 * ******************************************************************************
 * @brief 	: 根据电平状态和时间状态判断按键状态
 * @param 	: private_data [in/out], void类型指针，用来获取设备信息
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static void key_device_ops_update_status(void *private_data)
{
    key_device_t *p_dev = (key_device_t *)private_data;
    uint32_t time_current = HAL_GetTick();

    // check the key level
    p_dev->ops.update_level((void *)p_dev);
    if (p_dev->level_valid == p_dev->level_current && time_current > p_dev->time_next)
    {
        p_dev->status = KEY_STATUS_PRESS;
        // calculate the next time press the button
        p_dev->ops.update_time_next((void *)p_dev);
        // performs a callback function
        p_dev->call_func((void *)p_dev);
    }
    else
    {
        p_dev->status = KEY_STATUS_IDLE;
    }
}

/**
 * ******************************************************************************
 * @brief 	: key设备注册函数，初始化,设置回调函数
 * @param 	: p_func [in/out], 回调函数指针
 * @author 	: chenningzhan
 * @note	: 设置各个key设备的参数以及回调函数
 * ******************************************************************************
 */
void key_device_register(p_dev_call_func p_func)
{
    for (uint8_t i = 0; i < KEY_DEVICE_COUNT; i++)
    {
        // dynamic distribution device structure
        key_device_t *dev_source = malloc(sizeof(key_device_t));
        key_device_t *dev_destin = &g_key_devices[i];

        // set default data
        dev_source->gpio_port = g_key_device_resources[i].gpio_port;
        dev_source->pin = g_key_device_resources[i].pin;
        dev_source->index = g_key_device_resources[i].index;
        dev_source->level_valid = KEY_VALID_LEVEL;
        dev_source->level_current = !dev_source->level_valid;
        dev_source->time_next = 0;
        dev_source->time_interval = KEY_PRESS_INTERVAL_TIME;
        dev_source->status = KEY_STATUS_IDLE;

        // set operation function
        dev_source->ops.update_time_next = key_device_ops_update_time_next;
        dev_source->ops.update_level = key_device_ops_update_level;
        dev_source->ops.update_status = key_device_ops_update_status;
        dev_source->call_func = p_func;

        // copy data
        memcpy(dev_destin, dev_source, sizeof(key_device_t));
        // Frees dynamically allocated memory
        free(dev_source);
    }
}