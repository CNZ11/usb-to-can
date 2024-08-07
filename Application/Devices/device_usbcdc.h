#ifndef __DEVICE_USBCDC_H__
#define __DEVICE_USBCDC_H__

/* --------------------------- library header file -------------------------- */
#include "usbd_cdc_if.h"

/* ---------------------------- user header file ---------------------------- */
#include "./device.h"
#include "usb2can_def.h"

/* ---------------------------- macro definition ---------------------------- */
#define USBCDC_DEVICE_BUFF_SIZE_TX (USB2CAN_USBCDC2PC_LEN)
#define USBCDC_DEVICE_BUFF_SIZE_RX (USB2CAN_PC2USBCDC_LEN)

/* --------------------------- struct definitions --------------------------- */
/**
 * @brief Enumeration for the usbcdc device status.
 */
typedef enum
{
    USBCDC_STATUS_IDLE = 0X00,
    USBCDC_STATUS_BUSY_RECEIVE,
    USBCDC_STATUS_BUSY_SEND,
    USBCDC_STATUS_UNKNOWN,
} usbcdc_device_status_t;

/**
 * @brief Structure for the usbcdc device parameters.
 */
typedef struct
{
    usbcdc_device_status_t status;
    volatile uint16_t rx_len;
} usbcdc_device_params_t;

/**
 * @brief Structure for the usbcdc device operations.
 */
typedef struct
{
    void (*update_status)(void *p_self, usbcdc_device_status_t arg_status);
    usbcdc_device_status_t (*return_status)(void *p_self);
    void (*buff_send)(void *p_self);
    void (*buff_receive)(void *p_self, uint8_t *buff, uint32_t *len);
} usbcdc_device_ops_t;

/**
 * @brief Structure for the usbcdc device.
 */
typedef struct
{
    usbcdc_device_params_t params;
    uint8_t buff_tx[USBCDC_DEVICE_BUFF_SIZE_TX];
    uint8_t buff_rx[USBCDC_DEVICE_BUFF_SIZE_RX];
    usbcdc_device_ops_t ops;
    p_callback_func_dev call_func;
} usbcdc_device_t;

/* -------------------------- function declaration -------------------------- */

usbcdc_device_t *usbcdc_device_get_pointer(void);

void usbcdc_device_register(p_callback_func_dev p_func);

#endif
