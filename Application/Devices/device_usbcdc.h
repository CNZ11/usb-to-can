#ifndef __DEVICE_USBCDC_H__
#define __DEVICE_USBCDC_H__

/* ---------------------------- user header file ---------------------------- */
#include "./device.h"
#include "usb2can_def.h"

/* ---------------------------- macro definition ---------------------------- */
#define USBCDC_DEVICE_BUFF_SIZE_TX (USB2CAN_USBCDC2PC_LEN)
#define USBCDC_DEVICE_BUFF_SIZE_RX (USB2CAN_PC2USBCDC_LEN)

/* --------------------------- struct definitions --------------------------- */

typedef struct
{
    volatile uint16_t rx_len;
} usbcdc_device_params_t;

typedef struct
{
    void (*buff_send)(void *p_object);
    void (*buff_receive)(void *p_object, uint8_t *buff, uint32_t *len);
} usbcdc_device_ops_t;

typedef struct
{
    usbcdc_device_params_t params;
    uint8_t buff_tx[USBCDC_DEVICE_BUFF_SIZE_TX];
    uint8_t buff_rx[USBCDC_DEVICE_BUFF_SIZE_RX];
    usbcdc_device_ops_t ops;
    p_func_callback_dev call_func;
} usbcdc_device_t;

/* -------------------------- function declaration -------------------------- */
usbcdc_device_t *usbcdc_device_get_pointer(void);

void usbcdc_device_register(p_func_callback_dev p_func);

#endif
