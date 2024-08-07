#ifndef __DEVICE_CAN_H__
#define __DEVICE_CAN_H__

/* --------------------------- library header file -------------------------- */
#include "fdcan.h"

/* ---------------------------- user header file ---------------------------- */
#include "./device.h"
#include "usb2can_def.h"

/* --------------------------- struct definitions --------------------------- */
/**
 * @brief Enumeration for the can device status.
 */
typedef enum
{
    CAN_STATUS_IDLE = 0x00,
    CAN_STATUS_RECEIVE_SUCCESS,
    CAN_STATUS_RECEIVE_FAILED,
    CAN_STATUS_SENDOUT_SUCCESS,
    CAN_STATUS_SENDOUT_FAILED,
    CAN_STATUS_SETUP_SUCCESS,
    CAN_STATUS_SETUP_FAILED,
    CAN_STATUS_UNKNOWN,
} can_device_status_t;

typedef enum
{
    CAN_ID_FD1 = 0,
    CAN_ID_COUNT,
} can_device_index_t;

typedef struct
{
    can_device_status_t status;
    pc2usbcdc_fream_can_baudrate_index_t baudrate; // Baudrate index
} can_device_params_t;

typedef struct
{
    FDCAN_HandleTypeDef *hfdcan;
} can_device_resources_t;

typedef struct
{
    void (*enable)(void *private_data); // Function to initialize and enable CAN
    void (*reinit)(void *private_data); // Function to reinitialize baudrate
    void (*update_status)(void *private_data, can_device_status_t arg_status);
    can_device_status_t (*return_status)(void *private_data);
    void (*buff_send)(void *private_data);
} can_device_ops_t;

typedef struct
{
    uint8_t buff_tx[USB2CAN_CAN_DATA_LEN]; // Transmit buffer
    uint8_t buff_rx[USB2CAN_CAN_DATA_LEN]; // Receive buffer
    can_device_params_t params;            // Device parameters
    FDCAN_HandleTypeDef *hfdcan;           // Pointer to FDCAN handle
    FDCAN_TxHeaderTypeDef htx;             // CAN Tx header structure
    FDCAN_RxHeaderTypeDef hrx;             // CAN Rx header structure
    can_device_ops_t ops;                  // CAN device operations
    p_callback_func_dev call_func;         // Pointer to device callback function
} can_device_t;

/* -------------------------- function declaration -------------------------- */
can_device_t *can_device_get_pointer(can_device_index_t arg_index);

void can_device_register(p_callback_func_dev p_func);

#endif