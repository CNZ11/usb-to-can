/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: device_led.h
 * ******************************************************************************
 * @brief 	: led设备层的头文件，定义了led操作接口和相关数据结构
 * @version	: 2.0
 * @date 	: 2024-07-09
 * @note 	:
 * ******************************************************************************
 */

#ifndef __DEVICE_LED_H__
#define __DEVICE_LED_H__

/* ---------------------------- user header file ---------------------------- */
#include "main.h"

/* ---------------------------- macro definition ---------------------------- */
#define LED_DEVICE_COUNT 2             // Hardware Resources have led number
#define LED_VALID_LEVEL GPIO_PIN_RESET // the effective level at which they are lit

/* --------------------------- struct definitions --------------------------- */

typedef enum
{
    LED_ID_RED = 0,   //
    LED_ID_GREEN = 1, //
    LED_ID_COUNT,     // The actual number of leds used
} led_device_index_t;

typedef enum
{
    LED_STATUS_OFF = 0,
    LED_STATUS_ON = 1,
    LED_STATUS_UNKNOWN,
} led_device_status_t;

typedef struct
{
    GPIO_TypeDef *gpio_port;
    uint16_t pin;
    led_device_index_t index;
    GPIO_PinState valid_level;
} led_device_resources_t;

typedef struct
{
    void (*update_status)(void *private_data, led_device_status_t arg_status);
    void (*set_on)(void *private_data);
    void (*set_off)(void *private_data);
    void (*set_toggle)(void *private_data);
} led_device_ops_t;

typedef struct
{
    led_device_resources_t res;
    led_device_status_t status;
    led_device_ops_t ops;
    p_dev_call_func call_func;
} led_device_t;

/* -------------------------- function declaration -------------------------- */
led_device_t *led_device_get_pointer(led_device_index_t index);
void led_device_register(p_dev_call_func p_func);

#endif
