/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: device_usbcdc.c
 * ******************************************************************************
 * @brief 	: 【introduction】
 * @version	: 2.0
 * @date 	: 2024-07-25
 * @note 	: 【this file major modifications】
 * *
 * ******* Improvement *********
 * * 1. Added members to the device resource structure.
 * *
 * * 2. Optimize the code framework.
 * *
 * *
 * ******* Fix the bug *********
 * * 1. fix the bug of :
 * *
 * *
 * ******************************************************************************
 */

/* ---------------------------- user header file ---------------------------- */
#include "device_usbcdc.h"
#include "usbd_cdc_if.h"

/* ----------------------- global variable definition ----------------------- */
usbcdc_device_t g_usbcdc_device = {0};

/* ----------------------- public function definition ----------------------- */

/**
 * ******************************************************************************
 * @brief 	: get a pointer to an device
 * @retval 	: Pointer to the usbcdc_device_t structure.
 * @author 	: chenningzhan
 * @note	: An open interface for other modules to call to get a device pointer
 * ******************************************************************************
 */
usbcdc_device_t *usbcdc_device_get_pointer(void)
{
    return &g_usbcdc_device;
}

/* ------------------ private operation function definition ----------------- */

/**
 * ******************************************************************************
 * @brief 	: Sends data from the transmit buffer of the device.
 * @param 	  p_object  	: a pointer to get the device object
 * @author 	: chenningzhan
 * @note 	: None
 * ******************************************************************************
 */
static void usbcdc_device_ops_buff_send(void *p_object)
{
    usbcdc_device_t *p_dev = (usbcdc_device_t *)p_object;

    // send data to the USB device
    CDC_Transmit_FS(p_dev->buff_tx, USBCDC_DEVICE_BUFF_SIZE_TX);
}

/**
 * ******************************************************************************
 * @brief 	: Receives data into the receive buffer of the device.
 * @param 	  p_object  	: a pointer to get the device object
 * @param 	  buff  	: Pointer to the buffer where data will be stored.
 * @param 	  len  	: Pointer to the variable containing the length of received data.
 * @author 	: chenningzhan
 * @note 	: None
 * ******************************************************************************
 */
static void usbcdc_device_ops_buff_receive(void *p_object, uint8_t *buff, uint32_t *len)
{
    usbcdc_device_t *p_dev = (usbcdc_device_t *)p_object;

    p_dev->params.rx_len = *len;

    // check if length is beyond buffer size limit
    if (p_dev->params.rx_len > USBCDC_DEVICE_BUFF_SIZE_RX)
    {
        p_dev->params.rx_len = USBCDC_DEVICE_BUFF_SIZE_RX;
    }
    memset(p_dev->buff_rx, 0, USBCDC_DEVICE_BUFF_SIZE_RX);
    memcpy(p_dev->buff_rx, buff, p_dev->params.rx_len);
}

/* --------------------------- Initialize function -------------------------- */

/**
 * ******************************************************************************
 * @brief 	: register cdc device with its associated callback function.
 * @param 	  p_func  	: The callback function pointer for the device operations.
 * @author 	: chenningzhan
 * @note 	: None
 * ******************************************************************************
 */
void usbcdc_device_register(p_func_callback_dev p_func)
{
    // gets a pointer to the global variable device
    usbcdc_device_t *p_dev = &g_usbcdc_device;

    // clean the buffer
    memset(p_dev->buff_tx, 0, sizeof(p_dev->buff_tx));
    memset(p_dev->buff_rx, 0, sizeof(p_dev->buff_rx));

    // set default parameters
    p_dev->params.rx_len = 0;

    // set operation function
    p_dev->ops.buff_send = usbcdc_device_ops_buff_send;
    p_dev->ops.buff_receive = usbcdc_device_ops_buff_receive;

    // bind the callback function
    if (NULL_PTR == p_func || 0 == p_func)
    {
        p_dev->call_func = NULL_PTR;
    }
    else
    {
        p_dev->call_func = p_func;
    }
}
