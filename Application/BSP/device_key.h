/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: device_key.h
 * ******************************************************************************
 * @brief 	: key设备层的头文件，定义了key操作接口和相关数据结构
 * @version	: 2.0
 * @date 	: 2024-07-15
 * @note 	:
 * ******************************************************************************
 */

#ifndef __DEVICE_KEY_H__
#define __DEVICE_KEY_H__

/* ---------------------------- user header file ---------------------------- */
#include "main.h"

/* ---------------------------- macro definition ---------------------------- */
// the number of keys a
#define KEY_DEVICE_COUNT 1
// the effective level at which they are pressed
#define KEY_VALID_LEVEL GPIO_PIN_SET
// the key is pressed at an interval (ms)
#define KEY_PRESS_INTERVAL_TIME 200

/* --------------------------- struct definitions --------------------------- */

typedef enum
{
    KEY_ID_CTRL = 0,
} key_device_index_t;

typedef enum
{
    KEY_STATUS_IDLE = 0, // key is idle
    KEY_STATUS_PRESS,    // key is pressed
} key_device_status_t;

typedef struct
{
    GPIO_TypeDef *gpio_port;
    uint16_t pin;
    key_device_index_t index;
} key_device_resources_t;

typedef struct
{
    void (*update_time_next)(void *private_data);
    void (*update_level)(void *private_data);
    void (*update_status)(void *private_data);
} key_device_ops_t;

typedef struct
{
    GPIO_TypeDef *gpio_port;     // GPIO
    uint16_t pin;                // pin number
    GPIO_PinState level_valid;   //
    GPIO_PinState level_current; //
    key_device_status_t status;  //
    key_device_index_t index;    //
    uint32_t time_interval;      // the key is pressed at an interval (ms)
    uint32_t time_next;          // the next time to check the status
    key_device_ops_t ops;        //
    p_dev_call_func call_func;   // call back function
} key_device_t;

/* -------------------------- function declaration -------------------------- */
key_device_index_t key_device_return_status(void);
key_device_t *key_device_get_pointer(key_device_index_t index);
void key_device_register(p_dev_call_func p_func);

#endif /* __APP_KEY_H__ */