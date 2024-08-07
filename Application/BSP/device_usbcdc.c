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
 * * 1. improvement the function :
 * *
 * *
 * ******* Fix the bug *********
 * * 1. fix the bug of :
 * *
 * *
 * ******* HOW TO USE *********
 * * 0. add the files in this folder to current project
 * * 1. #include 'device_usbcdc.h'
 * * 2. change the macro and enum in 'device_usbcdc.h':
 * * 3. change the resources in 'device_usbcdc.c':
 * * 4. register the device and provides a callback function
 * * 5. Initialize,use a pointer to get an action handle, and use an action function
 * *
 * ******************************************************************************
 */
/* --------------------------- library header file -------------------------- */
#include <string.h>
/* ---------------------------- user header file ---------------------------- */
#include "device_usbcdc.h"

/* ----------------------- global variable definition ----------------------- */
usbcdc_device_t g_usbcdc_device = {0};

/* ----------------------- public function definition ----------------------- */

usbcdc_device_t *usbcdc_device_get_pointer(void)
{
    return &g_usbcdc_device;
}

/* ------------------ private operation function definition ----------------- */

/**
 * ******************************************************************************
 * @brief 	: Updates the status of the device.
 * @param 	: private_data [in/out], Pointer to the device structure.
 * @param 	: arg_status [in/out], The new status to be set.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static void usbcdc_device_ops_update_status(void *private_data, usbcdc_device_status_t arg_status)
{
    usbcdc_device_t *p_dev = (usbcdc_device_t *)private_data;
    if (arg_status <= USBCDC_STATUS_UNKNOWN && arg_status >= USBCDC_STATUS_IDLE)
    {
        p_dev->status = arg_status;
    }
    else
    {
        LOG_ERROR("the status unknown");
    }
}

/**
 * ******************************************************************************
 * @brief 	: Return the current status of the device.
 * @param 	: private_data [in/out], Pointer to the device structure.
 * @retval 	: The current status of the device.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static usbcdc_device_status_t usbcdc_device_ops_return_status(void *private_data)
{
    usbcdc_device_t *p_dev = (usbcdc_device_t *)private_data;

    return p_dev->status;
}

/**
 * ******************************************************************************
 * @brief 	: Sends data from the transmit buffer of the device.
 * @param 	: private_data [in/out], Pointer to the device structure.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static void usbcdc_device_ops_buff_send(void *private_data)
{
    usbcdc_device_t *p_dev = (usbcdc_device_t *)private_data;

    p_dev->ops.update_status((void *)p_dev, USBCDC_STATUS_BUSY_SEND);
    CDC_Transmit_FS(p_dev->buff_tx, USBCDC_DEVICE_BUFF_SIZE_TX);
    p_dev->ops.update_status((void *)p_dev, USBCDC_STATUS_IDLE);
}

/**
 * ******************************************************************************
 * @brief 	: Receives data into the receive buffer of the device.
 * @param 	: private_data [in/out], Pointer to the device structure.
 * @param 	: Buf [in/out], Pointer to the buffer where data will be stored.
 * @param 	: Len [in/out], Pointer to the variable containing the length of received data.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static void usbcdc_device_ops_buff_receive(void *private_data, uint8_t *Buf, uint32_t *Len)
{
    usbcdc_device_t *p_dev = (usbcdc_device_t *)private_data;

    p_dev->ops.update_status((void *)p_dev, USBCDC_STATUS_BUSY_RECEIVE);
    p_dev->rx_len = *Len;
    // check if length is beyond buffer size limit
    if (p_dev->rx_len > ARRAY_NUM(p_dev->buff_rx))
    {
        p_dev->rx_len = ARRAY_NUM(p_dev->buff_rx);
    }
    else
    {
        memset(p_dev->buff_rx, 0, ARRAY_NUM(p_dev->buff_rx));
        memcpy(p_dev->buff_rx, Buf, p_dev->rx_len);
    }
    p_dev->ops.update_status((void *)p_dev, USBCDC_STATUS_IDLE);
}

/* --------------------------- Initialize function -------------------------- */

/**
 * ******************************************************************************
 * @brief 	: Registers the device with its operations and callback function.
 * @param 	  p_func  	: The callback function pointer for the device operations.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
void usbcdc_device_register(p_dev_call_func p_func)
{
    // dynamic distribution device structure
    usbcdc_device_t *dev_source = malloc(sizeof(usbcdc_device_t));
    usbcdc_device_t *dev_destin = &g_usbcdc_device;

    // clean the buffer
    memset(dev_source->buff_rx, 0, ARRAY_NUM(dev_source->buff_rx));
    memset(dev_source->buff_tx, 0, ARRAY_NUM(dev_source->buff_tx));

    // set default parameters
    dev_source->status = USBCDC_STATUS_IDLE;

    // set operation function
    dev_source->ops.buff_send = usbcdc_device_ops_buff_send;
    dev_source->ops.buff_receive = usbcdc_device_ops_buff_receive;
    dev_source->ops.update_status = usbcdc_device_ops_update_status;
    dev_source->ops.return_status = usbcdc_device_ops_return_status;
    if ((void *)NULL != p_func)
    {
        dev_source->call_func = p_func;
    }
    else
    {
        dev_source->call_func = (void *)NULL;
    }

    // copy data
    memcpy(dev_destin, dev_source, sizeof(usbcdc_device_t));
    // frees dynamically allocated memory
    free(dev_source);
}
