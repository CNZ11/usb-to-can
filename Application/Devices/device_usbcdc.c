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

/* ---------------------------- user header file ---------------------------- */
#include "device_usbcdc.h"

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
 * @brief 	: Updates the status of the device.
 * @param 	  p_self  	: a pointer to get the device object
 * @param 	: arg_status [in/out], The new status to be set.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static void usbcdc_device_ops_update_status(void *p_self, usbcdc_device_status_t arg_status)
{
    usbcdc_device_t *p_dev = (usbcdc_device_t *)p_self;
    if (arg_status <= USBCDC_STATUS_UNKNOWN && arg_status >= USBCDC_STATUS_IDLE)
    {
        p_dev->params.status = arg_status;
    }
    else
    {
        LOG_ERROR("the status unknown");
    }
}

/**
 * ******************************************************************************
 * @brief 	: Return the current status of the device.
 * @param 	  p_self  	: a pointer to get the device object
 * @retval 	: The current status of the device.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static usbcdc_device_status_t usbcdc_device_ops_return_status(void *p_self)
{
    usbcdc_device_t *p_dev = (usbcdc_device_t *)p_self;

    return p_dev->params.status;
}

/**
 * ******************************************************************************
 * @brief 	: Sends data from the transmit buffer of the device.
 * @param 	  p_self  	: a pointer to get the device object
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static void usbcdc_device_ops_buff_send(void *p_self)
{
    usbcdc_device_t *p_dev = (usbcdc_device_t *)p_self;

    // update the device status
    p_dev->ops.update_status((void *)p_dev, USBCDC_STATUS_BUSY_SEND);

    // send data to the USB device
    CDC_Transmit_FS(p_dev->buff_tx, USBCDC_DEVICE_BUFF_SIZE_TX);

    // reset the device status to idle
    p_dev->ops.update_status((void *)p_dev, USBCDC_STATUS_IDLE);
}

/**
 * ******************************************************************************
 * @brief 	: Receives data into the receive buffer of the device.
 * @param 	  p_self  	: a pointer to get the device object.
 * @param 	  buff  	: Pointer to the buffer where data will be stored.
 * @param 	  len  	: Pointer to the variable containing the length of received data.
 * @author 	: chenningzhan
 * @note 	: None
 * ******************************************************************************
 */
static void usbcdc_device_ops_buff_receive(void *p_self, uint8_t *buff, uint32_t *len)
{
    usbcdc_device_t *p_dev = (usbcdc_device_t *)p_self;

    p_dev->ops.update_status((void *)p_dev, USBCDC_STATUS_BUSY_RECEIVE);
    p_dev->params.rx_len = *len;

    // check if length is beyond buffer size limit
    if (p_dev->params.rx_len > USBCDC_DEVICE_BUFF_SIZE_RX)
    {
        p_dev->params.rx_len = USBCDC_DEVICE_BUFF_SIZE_RX;
    }
    else
    {
        memset(p_dev->buff_rx, 0, USBCDC_DEVICE_BUFF_SIZE_RX);
        memcpy(p_dev->buff_rx, buff, p_dev->params.rx_len);
    }

    // update the device status
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
void usbcdc_device_register(p_callback_func_dev p_func)
{
    // gets a pointer to the global variable device
    usbcdc_device_t *p_dev = &g_usbcdc_device;

    // clean the buffer
    memset(p_dev->buff_rx, 0, USBCDC_DEVICE_BUFF_SIZE_RX);
    memset(p_dev->buff_tx, 0, USBCDC_DEVICE_BUFF_SIZE_TX);

    // set default parameters
    p_dev->params.status = USBCDC_STATUS_IDLE;
    p_dev->params.rx_len = 0;

    // set operation function
    p_dev->ops.buff_send = usbcdc_device_ops_buff_send;
    p_dev->ops.buff_receive = usbcdc_device_ops_buff_receive;
    p_dev->ops.update_status = usbcdc_device_ops_update_status;
    p_dev->ops.return_status = usbcdc_device_ops_return_status;

    // bind the callback function
    if ((void *)NULL != p_func)
    {
        p_dev->call_func = p_func;
    }
    else
    {
        p_dev->call_func = (void *)NULL;
    }
}
