#ifndef __DEVICE_BUZZER_H__
#define __DEVICE_BUZZER_H__

/* ---------------------------- user header file ---------------------------- */
#include "./device.h"

/* --------------------------- library header file -------------------------- */
#include "tim.h"

/* --------------------------- struct definitions --------------------------- */

/**
 * @brief Structure to hold the hardware resources of a buzzer device.
 */
typedef struct
{
    TIM_HandleTypeDef *timer;
    uint32_t channel;
} buzzer_device_resources_t;

/**
 * @brief Structure to hold the parameters and state of a buzzer device.
 */
typedef struct
{
    uint32_t ccr;
} buzzer_device_params_t;

/**
 * @brief Structure to hold the operations for a buzzer device.
 */
typedef struct
{
    void (*init)(void *p_this);
    void (*tick)(void *p_this, uint8_t count);
} buzzer_device_ops_t;

/**
 * @brief Structure to hold the device object for a buzzer.
 */
typedef struct
{
    const buzzer_device_resources_t *res;
    buzzer_device_params_t params;
    buzzer_device_ops_t ops;
    p_func_callback_dev call_func;
} buzzer_device_t;

/* -------------------------- function declaration -------------------------- */

buzzer_device_t *buzzer_device_get_pointer(void);

void buzzer_device_register(p_func_callback_dev p_func);

#endif
