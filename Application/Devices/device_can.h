#ifndef __DEVICE_CAN_H__
#define __DEVICE_CAN_H__

/* ---------------------------- user header file ---------------------------- */
#include "./device.h"

/* --------------------------- library header file -------------------------- */

#include "fdcan.h"

/* ---------------------------- macro definition ---------------------------- */

#define CAN_DEVICE_BUFF_SIZE_TX (8)
#define CAN_DEVICE_BUFF_SIZE_RX (8)

/* --------------------------- struct definitions --------------------------- */

/**
 * @brief Enumeration for the can device index.
 */
typedef enum
{
    CAN_ID_FD1 = 0,
    CAN_ID_COUNT,
} can_device_index_e;

/**
 * @brief Enumeration for the can device status.
 */
typedef enum
{
    CAN_STATUS_IDLE = 0,
    CAN_STATUS_BUSY,
} can_device_status_e;

/**
 * @brief Enumeration for PC to set can baud rate
 */
typedef enum
{
    CAN_BAUDRATE_UNKNOW = -1,
    CAN_BAUDRATE_1M = 0,
    CAN_BAUDRATE_500K = 3,
    CAN_BAUDRATE_250K = 5,
    CAN_BAUDRATE_200K = 6,
    CAN_BAUDRATE_125K = 7,
    CAN_BAUDRATE_100K = 8,
    CAN_BAUDRATE_TYPE,
} can_device_baudrate_index_e;

/**
 * @brief Structure for the can device parameters.
 */
typedef struct
{
    can_device_baudrate_index_e baudrate; // Baudrate index
    can_device_status_e status;           //
} can_device_params_t;

/**
 * @brief Structure for the can device resources.
 */
typedef struct
{
    FDCAN_HandleTypeDef *hfdcan;
} can_device_resources_t;

/**
 * @brief Structure for the can device operations.
 */
typedef struct
{
    void (*enable)(void *p_object);
    void (*reinit)(void *p_object);
    void (*buff_send)(void *p_object);
    void (*update_status)(void *p_object, can_device_status_e status);
    can_device_status_e (*return_status)(void *p_object);
} can_device_ops_t;

/**
 * @brief Structure for the can device.
 */
typedef struct
{
    uint8_t buff_tx[CAN_DEVICE_BUFF_SIZE_TX]; // Transmit buffer
    uint8_t buff_rx[CAN_DEVICE_BUFF_SIZE_RX]; // Receive buffer
    const can_device_resources_t *res;        // Pointer to FDCAN handle
    FDCAN_TxHeaderTypeDef htx;                // CAN Tx header structure
    FDCAN_RxHeaderTypeDef hrx;                // CAN Rx header structure
    can_device_params_t params;               //
    can_device_ops_t ops;                     // CAN device operations
    p_func_callback_dev call_func;            // Pointer to device callback function
} can_device_t;

/* -------------------------- function declaration -------------------------- */
can_device_t *can_device_get_pointer(can_device_index_e arg_index);

void can_device_register(p_func_callback_dev p_func);

#endif
