#ifndef __DEVICE_USB2CAN_H__
#define __DEVICE_USB2CAN_H__

/* ---------------------------- user header file ---------------------------- */
#include "./device.h"
#include "usb2can_def.h"


/* --------------------------- struct definitions --------------------------- */

typedef enum
{
    USB2CAN_STATUS_IDLE = 0,
    USB2CAN_STATUS_BUSY,
} usb2can_device_status_e;

/**
 * @brief 	: Define the type of action for the USB2CAN device
 * @note	: Based on the host computer's data type, as well as their last behavior to make decisions
 */
typedef enum
{
    USB2CAN_ACTION_PARSE = 0,    // parse the packet from the pc
    USB2CAN_ACTION_CAN_BAUDRATE, // fix the can baud rate
    USB2CAN_ACTION_CAN_STOP,     // stop the transmit to can bus
    USB2CAN_ACTION_CAN_TX,       // transmit the packet from usb cdc to the can bus
    USB2CAN_ACTION_FB_HEARTBEAT, // return the packet to pc for heartbeat
    USB2CAN_ACTION_FB_CAN_TX,    // return the packet to pc for can packet
    USB2CAN_ACTION_FB_CAN_RX,    // return the packet to pc for can packet
    USB2CAN_ACTION_UNKNOWN,      //
} usb2can_device_action_e;

typedef struct
{
    usb2can_device_action_e action;
    usb2can_device_status_e status;
} usb2can_device_params_t;

typedef struct
{
    void (*update_action)(void *p_object, usb2can_device_action_e action);
    usb2can_device_action_e (*return_action)(void *p_object);
} usb2can_device_ops_t;

typedef struct
{
    usb2can_device_params_t params;
    usb2can_device_ops_t ops;
    p_func_callback_dev call_func;
} usb2can_device_t;

/* -------------------------- function declaration -------------------------- */

usb2can_device_t *usb2can_device_get_pointer(void);

void usb2can_device_register(p_func_callback_dev p_func);

#endif

