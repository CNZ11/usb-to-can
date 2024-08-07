#ifndef __DEVICE_USBCDC_H__
#define __DEVICE_USBCDC_H__

/* --------------------------- library header file -------------------------- */
#include "usbd_cdc_if.h"

/* ---------------------------- user header file ---------------------------- */
#include "main.h"
#include "usb2can_def.h"

/* ---------------------------- macro definition ---------------------------- */
#define USBCDC_DEVICE_BUFF_SIZE_TX (USB2CAN_USBCDC2PC_LEN)
#define USBCDC_DEVICE_BUFF_SIZE_RX (USB2CAN_PC2USBCDC_LEN)

/* --------------------------- struct definitions --------------------------- */

typedef enum
{
    USBCDC_STATUS_IDLE = 0X00,
    USBCDC_STATUS_BUSY_RECEIVE,
    USBCDC_STATUS_BUSY_SEND,
    USBCDC_STATUS_UNKNOWN,
} usbcdc_device_status_t;

typedef struct
{
    void (*update_status)(void *private_data, usbcdc_device_status_t arg_status);
    usbcdc_device_status_t (*return_status)(void *private_data);
    void (*buff_send)(void *private_data);
    void (*buff_receive)(void *private_data, uint8_t *Buf, uint32_t *Len);
} usbcdc_device_ops_t;

typedef struct
{
    volatile uint16_t rx_len;
    uint8_t buff_tx[USBCDC_DEVICE_BUFF_SIZE_TX];
    uint8_t buff_rx[USBCDC_DEVICE_BUFF_SIZE_RX];
    volatile usbcdc_device_status_t status;
    usbcdc_device_ops_t ops;
    p_dev_call_func call_func;
} usbcdc_device_t;

/* -------------------------- function declaration -------------------------- */
usbcdc_device_t *usbcdc_device_get_pointer(void);

void usbcdc_device_register(p_dev_call_func p_func);

#endif
