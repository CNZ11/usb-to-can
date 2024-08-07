/**
 * ******************************************************************************

 * @file 	: device_can.c
 * ******************************************************************************
 * @brief 	: 【this file introduction】
 * @version	: 2.0
 * @date 	: 2024-07-25
 * @note 	: 【this file major modifications】
 * *
 * ******* Improvement *********
 * * 1. Added members to the device resource structure.
 * *
 * * 2. Optimize the code framework.
 * *
 * ******* Fix the bug *********
 * * 1. fix the bug of device callback:
 * *
 * *
 * ******* HOW TO USE *********
 * * 0. add the files in this folder to current project
 * * 1. #include 'device_can.h'
 * * 2. change the macro and enum in 'device_can.h':
 * * 3. change the resources in 'device_can.c':
 * * 4. register the device and provides a callback function
 * * 5. Use a pointer to get an action handle, and use an action function
 * *
 * ******************************************************************************
 */

/* ---------------------------- user header file ---------------------------- */
#include "device_can.h"

/* ----------------------- global variable definition ----------------------- */
can_device_t g_can_device[CAN_ID_COUNT] = {0};
const can_device_resources_t g_can_device_resources[CAN_ID_COUNT] = {
    &hfdcan1,
};

/* ----------------------- public function definition ----------------------- */

/**
 * ******************************************************************************
 * @brief 	: get a pointer to an device
 * @param 	: arg_index [in/out], The index of the led device.
 * @retval 	: Pointer to the can_device_t structure.
 * @author 	: chenningzhan
 * @note	: If the index is out of range, it logs an error and returns NULL.
 * @note	: An open interface for other modules to call to get a device pointer
 * ******************************************************************************
 */
can_device_t *can_device_get_pointer(can_device_index_t arg_index)
{
    if (arg_index < 0 || arg_index >= CAN_ID_COUNT)
    {
        LOG_ERROR("get pointer index out of range");
        return (void *)NULL;
    }
    else
    {
        return &g_can_device[arg_index];
    }
}

/* ------------------ private operation function definition ----------------- */

/**
 * ******************************************************************************
 * @brief 	: Update the status of the device.
 * @param 	: private_data [in/out], Pointer to the device structure.
 * @param 	: arg_status [in/out], The new status to be set.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static void can_device_ops_update_status(void *private_data, can_device_status_t arg_status)
{
    can_device_t *p_dev = (can_device_t *)private_data;
    if (arg_status <= CAN_STATUS_UNKNOWN && arg_status >= CAN_STATUS_IDLE)
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
 * @param 	: private_data [in/out], Pointer to the device structure.
 * @retval 	: The current status of the device.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static can_device_status_t can_device_ops_return_status(void *private_data)
{
    can_device_t *p_dev = (can_device_t *)private_data;

    return p_dev->params.status;
}

/**
 * ******************************************************************************
 * @brief 	: Initializes and enables the CAN device.
 * @param 	: private_data [in/out], Pointer to the device structure.
 * @author 	: chenningzhan
 * @note	: This function sets up the CAN filter and starts the CAN peripheral.
 * ******************************************************************************
 */
static void can_device_ops_enable(void *private_data)
{
    can_device_t *p_dev = (can_device_t *)private_data;

    // initialize the can filter
    FDCAN_FilterTypeDef fdcan1_filter;
    fdcan1_filter.IdType = FDCAN_STANDARD_ID;
    fdcan1_filter.FilterIndex = 0;
    fdcan1_filter.FilterType = FDCAN_FILTER_RANGE;
    fdcan1_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    fdcan1_filter.FilterID1 = 0x0000;
    fdcan1_filter.FilterID2 = 0x0000;
    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &fdcan1_filter) != HAL_OK)
    {
        Error_Handler();
    }
    // enable the can
    HAL_FDCAN_Start(p_dev->hfdcan);
    HAL_FDCAN_ActivateNotification(p_dev->hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}

/**
 * ******************************************************************************
 * @brief 	: Reinitializes the CAN baud rate.
 * @param 	: private_data [in/out], Pointer to the device structure.
 * @author 	: chenningzhan
 * @note	: This function sets the baud rate and reinitializes the CAN peripheral.
 * ******************************************************************************
 */
static void can_device_ops_reinit(void *private_data)
{
    can_device_t *p_dev = (can_device_t *)private_data;

    // Set the baud rate clock prescaler based on the current baud rate
    switch (p_dev->params.baudrate)
    {
    case CAN_BAUDRATE_1M:
    default:
        p_dev->hfdcan->Init.ClockDivider = FDCAN_CLOCK_DIV2;
        break;
    case CAN_BAUDRATE_500K:
        p_dev->hfdcan->Init.ClockDivider = FDCAN_CLOCK_DIV4;
        break;
    case CAN_BAUDRATE_250K:
        p_dev->hfdcan->Init.ClockDivider = FDCAN_CLOCK_DIV8;
        break;
    case CAN_BAUDRATE_200K:
        p_dev->hfdcan->Init.ClockDivider = FDCAN_CLOCK_DIV10;
        break;
    case CAN_BAUDRATE_125K:
        p_dev->hfdcan->Init.ClockDivider = FDCAN_CLOCK_DIV16;
        break;
    case CAN_BAUDRATE_100K:
        p_dev->hfdcan->Init.ClockDivider = FDCAN_CLOCK_DIV20;
        break;
    }
    // Reinitialize the CAN peripheral with the new baud rate
    if (HAL_FDCAN_Init(p_dev->hfdcan) != HAL_OK)
    {
        LOG_ERROR("Failed to reinit motor can device");

        // update the device status
        p_dev->ops.update_status((void *)p_dev, CAN_STATUS_SETUP_FAILED);
    }
    else
    {
        // restart the device
        HAL_FDCAN_Start(p_dev->hfdcan);
        LOG_DEBUG("reinit motor can device success");

        // update the device status
        p_dev->ops.update_status((void *)p_dev, CAN_STATUS_SETUP_SUCCESS);
    }
}

static void can_device_ops_buff_send(void *private_data)
{
    can_device_t *p_dev = (can_device_t *)private_data;

    if (HAL_FDCAN_AddMessageToTxFifoQ(p_dev->hfdcan, &p_dev->htx, p_dev->buff_tx) != HAL_OK)
    {
        Error_Handler();

        // update the device status
        p_dev->ops.update_status((void *)p_dev, CAN_STATUS_SENDOUT_FAILED);
    }
    else
    {
        p_dev->ops.update_status((void *)p_dev, CAN_STATUS_SENDOUT_SUCCESS);
    }
}

/* --------------------------- Initialize function -------------------------- */

/**
 * ******************************************************************************
 * @brief 	: Register the device with its operations.
 * @param 	: private_data [in/out], Pointer to the device callback function.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
void can_device_register(p_callback_func_dev p_func)
{
    for (uint8_t i = 0; i < CAN_ID_COUNT; i++)
    {
        // gets a pointer to the global variable device
        can_device_t *p_dev = &g_can_device[i];

        // clean the buffer
        memset(p_dev->buff_rx, 0, USB2CAN_CAN_DATA_LEN);
        memset(p_dev->buff_tx, 0, USB2CAN_CAN_DATA_LEN);

        // bind the hardware resource
        p_dev->hfdcan = g_can_device_resources[i].hfdcan;

        // set default parameters
        p_dev->params.baudrate = CAN_BAUDRATE_1M;
        p_dev->params.status = CAN_STATUS_IDLE;
        p_dev->htx.Identifier = 0x1FF;
        p_dev->htx.IdType = FDCAN_STANDARD_ID;
        p_dev->htx.TxFrameType = FDCAN_DATA_FRAME;
        p_dev->htx.DataLength = FDCAN_DLC_BYTES_8;
        p_dev->htx.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
        p_dev->htx.BitRateSwitch = FDCAN_BRS_OFF;
        p_dev->htx.FDFormat = FDCAN_FD_CAN;
        p_dev->htx.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
        p_dev->htx.MessageMarker = 0;

        // set operation function
        p_dev->ops.enable = can_device_ops_enable;
        p_dev->ops.reinit = can_device_ops_reinit;
        p_dev->ops.buff_send = can_device_ops_buff_send;
        p_dev->ops.update_status = can_device_ops_update_status;
        p_dev->ops.return_status = can_device_ops_return_status;

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
}