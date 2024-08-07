/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: device_buzzer.h
 * ******************************************************************************
 * @brief 	: buzzer设备层的头文件，定义了buzzer操作接口和相关数据结构
 * @version	: 1.0
 * @date 	: 2024-07-20
 * @note 	:
 * ******************************************************************************
 */
#ifndef __DEVICE_BUZZER_H__
#define __DEVICE_BUZZER_H__

/* ---------------------------- user header file ---------------------------- */
#include "main.h"
#include "tim.h"

/* ---------------------------- macro definition ---------------------------- */

/* --------------------------- struct definitions --------------------------- */

typedef enum
{
    BUZZER_STATUS_OFF = 0,
    BUZZER_STATUS_ON = 1,
} buzzer_status_t;

typedef struct
{
    TIM_HandleTypeDef *timer;
    uint32_t channel;
    uint32_t ccr;
} buzzer_device_resources_t;

typedef struct
{
    void (*set_on)(void *private_data);
    void (*set_off)(void *private_data);
    void (*set_toggle)(void *private_data);
} buzzer_device_ops_t;

typedef struct
{
    TIM_HandleTypeDef *timer; // user the timer pwm driver
    uint32_t channel;
    uint32_t ccr;
    buzzer_status_t status;
    buzzer_device_ops_t ops;
    p_dev_call_func call_func;
} buzzer_device_t;

/* -------------------------- function declaration -------------------------- */
buzzer_device_t *buzzer_device_get_pointer(void);
void buzzer_device_register(p_dev_call_func p_func);

#endif
