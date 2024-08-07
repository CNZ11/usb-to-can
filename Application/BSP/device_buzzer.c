/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 ��ݸ�б�ĩ�Ƽ����޹�˾
 * @file 	: device_buzzer.c
 * ******************************************************************************
 * @brief 	: �������豸��ʵ�֣������ṩ�豸ops�����ӿ�
 * @version	: 1.0
 * @date 	: 2024-07-20
 * @note 	: ʹ�÷��������� buzzer_device_register ��ʼ���������豸
 *            ͨ��buzzer_device_get_pointer��ȡ�豸������п���
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
 * @brief 	: buzzer�豸���������ӿ�
 * @param 	: private_data [in/out], void����ָ�룬������ȡ�豸��Ϣ
 * @author 	: chenningzhan
 * @note	: ���ݷ��������ͣ���Դ����Դ��������GPIO��ƽ������PWM�ź�
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
 * @brief 	: buzzer�豸�رղ����ӿ�
 * @param 	: private_data [in/out], void����ָ�룬������ȡ�豸��Ϣ
 * @author 	: chenningzhan
 * @note	: ���ݷ��������ͣ���Դ����Դ��������GPIO��ƽ��ر�PWM�ź�
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
 * @brief 	: ��ȡbuzzer�豸���
 * @retval 	: ָ���豸�ṹ���ָ��
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
 * @brief 	: buzzer �豸ע�ắ������ʼ��,���ûص�����
 * @param 	: p_func [in/out], �ص�����ָ��
 * @author 	: chenningzhan
 * @note	: ���ø���buzzer�豸�Ĳ����Լ��ص�����
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
