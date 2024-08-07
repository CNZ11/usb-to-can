#ifndef __DEVICE_USB2CAN_H__
#define __DEVICE_USB2CAN_H__

/* ---------------------------- user header file ---------------------------- */
#include "./device.h"
#include "usb2can_def.h"

/* --------------------------- struct definitions --------------------------- */

/**
 * @brief Enumeration for the action for the USB2CAN device
 * @note  Based on the host computer's data type, as well as their last behavior to make decisions
 */
typedef enum
{
    USB2CAN_ACTION_IDLE = 0x00,
    USB2CAN_ACTION_PARSE,              // parse the packet from the pc
    USB2CAN_ACTION_OPS_BAUDRATE,       // fix the can baud rate
    USB2CAN_ACTION_OPS_STOP,           // stop the transmit to can bus
    USB2CAN_ACTION_OPS_TX2CAN,         // transmit the packet from usb cdc to the can bus
    USB2CAN_ACTION_FB_TX2PC_HEARTBEAT, // return the packet to pc for heartbeat
    USB2CAN_ACTION_FB_TX2PC_CAN,       // return the packet to pc for can packet
    USB2CAN_ACTION_UNKNOWN,            //
} usb2can_device_action_t;

/**
 * @brief Structure for the usb2can device operations.
 */
typedef struct
{
    void (*update_action)(void *private_data, usb2can_device_action_t action);
    usb2can_device_action_t (*return_action)(void *private_data);
} usb2can_device_ops_t;

/**
 * @brief Structure for the usb2can device.
 */
typedef struct
{
    usb2can_device_action_t action;
    usb2can_device_ops_t ops;
    p_callback_func_dev call_func;
} usb2can_device_t;

/* -------------------------- function declaration -------------------------- */

usb2can_device_t *usb2can_device_get_pointer(void);

void usb2can_device_register(p_callback_func_dev p_func);

#endif
