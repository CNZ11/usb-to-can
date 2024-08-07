/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 ��ݸ�б�ĩ�Ƽ����޹�˾
 * @file 	: device_key.c
 * ******************************************************************************
 * @brief 	: the key device layer provides the ops operation interface of the key device
 * @version	: 2.0
 * @date 	: 2024-07-15
 * @note 	: ����û�г����������е�������������ܣ�ʹ��systickʱ��ʵ�֣�
 *            ����ͬ���ȴ������¼������������¼��ڻص���������ʹ��
 *            ����������ѯ����״̬��ͨ������ key_device_ask_status ��������
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
 * @brief 	: ͨ���豸id������ȡ�豸���
 * @param 	: index [in/out], �豸����
 * @retval 	: ָ���豸�ṹ���ָ�룬���������Ч�򷵻�NULL
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
 * @brief 	: ������ѯ״̬�������ذ��µİ�����ID
 * @retval 	: �а��°���������ID�����򷵻�-1
 * @author 	: chenningzhan
 * @note	: �������ļ����ã���ѯÿ������״̬
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
 * @brief 	: ������һ�ΰ������µ�ʱ��
 * @param 	: private_data [in/out], void����ָ�룬������ȡ�豸��Ϣ
 * @author 	: chenningzhan
 * @note	: ͨ��systickʱ������ʱ����
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
 * @brief 	: ͨ��������ѯ��ʽ��ȡ������ƽ
 * @param 	: private_data [in/out], void����ָ�룬������ȡ�豸��Ϣ
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
 * @brief 	: ���ݵ�ƽ״̬��ʱ��״̬�жϰ���״̬
 * @param 	: private_data [in/out], void����ָ�룬������ȡ�豸��Ϣ
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
 * @brief 	: key�豸ע�ắ������ʼ��,���ûص�����
 * @param 	: p_func [in/out], �ص�����ָ��
 * @author 	: chenningzhan
 * @note	: ���ø���key�豸�Ĳ����Լ��ص�����
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