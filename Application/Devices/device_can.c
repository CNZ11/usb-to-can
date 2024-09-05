
/* ---------------------------- user header file ---------------------------- */

#include "device_can.h"

/* ----------------------- global variable definition ----------------------- */

const can_device_resources_t g_can_device_resources[CAN_ID_COUNT] = {
    &hfdcan1,
};

can_device_t g_can_device[CAN_ID_COUNT] = {};

/* ----------------------- public function definition ----------------------- */

/**
 * ******************************************************************************
 * @brief 	: get a pointer to an device
 * @param 	  index  	: The index of the led device.
 * @retval 	: Pointer to the can_device_t structure.
 * @author 	: chenningzhan
 * @note	: If the index is out of range, it logs an error and returns NULL.
 *            An open interface for other modules to call to get a device pointer
 * ******************************************************************************
 */
can_device_t *can_device_get_pointer(can_device_index_e index)
{
    if (index < 0 || index >= CAN_ID_COUNT)
    {
        return NULL_PTR;
    }
    else
    {
        return &g_can_device[index];
    }
}

/* ------------------ private operation function definition ----------------- */

static void can_device_ops_update_status(void *p_object, can_device_status_e status)
{
    can_device_t *p_dev = (can_device_t *)p_object;

    p_dev->params.status = status;
}

static can_device_status_e can_device_ops_return_status(void *p_object)
{
    can_device_t *p_dev = (can_device_t *)p_object;

    return p_dev->params.status;
}

/**
 * ******************************************************************************
 * @brief 	: Initializes and enables the CAN device.
 * @param 	  p_self  	: a pointer to get the device object
 * @author 	: chenningzhan
 * @note	: This function sets up the CAN filter and starts the CAN peripheral.
 * ******************************************************************************
 */
static void can_device_ops_enable(void *p_self)
{
    can_device_t *p_dev = (can_device_t *)p_self;

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
    HAL_FDCAN_Start(p_dev->res->hfdcan);
    HAL_FDCAN_ActivateNotification(p_dev->res->hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}

/**
 * ******************************************************************************
 * @brief 	: Reinitializes the CAN baud rate.
 * @param 	  p_object  	: a pointer to get the device object
 * @author 	: chenningzhan
 * @note	: This function sets the baud rate and reinitializes the CAN peripheral.
 * ******************************************************************************
 */
static void can_device_ops_reinit(void *p_object)
{
    can_device_t *p_dev = (can_device_t *)p_object;

    uint32_t clk = FDCAN_CLOCK_DIV2;
    switch (p_dev->params.baudrate)
    {
    case CAN_BAUDRATE_500K:
        clk = FDCAN_CLOCK_DIV4;
        break;
    case CAN_BAUDRATE_250K:
        clk = FDCAN_CLOCK_DIV8;
        break;
    case CAN_BAUDRATE_200K:
        clk = FDCAN_CLOCK_DIV10;
        break;
    case CAN_BAUDRATE_125K:
        clk = FDCAN_CLOCK_DIV16;
        break;
    case CAN_BAUDRATE_100K:
        clk = FDCAN_CLOCK_DIV20;
        break;
    case CAN_BAUDRATE_1M:
    case CAN_BAUDRATE_UNKNOW:
    default:
        clk = FDCAN_CLOCK_DIV2;
        break;
    }

    p_dev->res->hfdcan->Init.ClockDivider = clk;
    if (HAL_FDCAN_Init(p_dev->res->hfdcan) != HAL_OK)
    {
        Error_Handler();
    }
    else
    {
        // restart the device
        HAL_FDCAN_Start(p_dev->res->hfdcan);
    }
}

static void can_device_ops_buff_send(void *p_object)
{
    can_device_t *p_dev = (can_device_t *)p_object;

    if (HAL_FDCAN_AddMessageToTxFifoQ(p_dev->res->hfdcan, &p_dev->htx, p_dev->buff_tx) != HAL_OK)
    {
        Error_Handler();
    }
    else
    {
        while (__HAL_FDCAN_GET_FLAG(p_dev->res->hfdcan, FDCAN_FLAG_TX_FIFO_EMPTY) == RESET)
            ;
    }
}

/* --------------------------- Initialize function -------------------------- */

/**
 * ******************************************************************************
 * @brief 	: Register the device with its operations.
 * @param 	  p_func  	: The callback function pointer for the device operations.
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
void can_device_register(p_func_callback_dev p_func)
{
    for (uint8_t i = 0; i < CAN_ID_COUNT; i++)
    {
        // gets a pointer to the global variable device
        can_device_t *p_dev = &g_can_device[i];

        // clean the buffer
        memset(p_dev->buff_tx, 0, sizeof(p_dev->buff_tx));
        memset(p_dev->buff_rx, 0, sizeof(p_dev->buff_rx));

        // bind the hardware resource
        p_dev->res = &(g_can_device_resources[i]);

        // set default parameters
        p_dev->params.baudrate = CAN_BAUDRATE_1M;
        p_dev->params.status = CAN_STATUS_IDLE;

        p_dev->htx.Identifier = 0x1FF;
        p_dev->htx.IdType = FDCAN_STANDARD_ID;
        p_dev->htx.TxFrameType = FDCAN_DATA_FRAME;
        p_dev->htx.DataLength = FDCAN_DLC_BYTES_8;
        p_dev->htx.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
        p_dev->htx.BitRateSwitch = FDCAN_BRS_OFF;
        p_dev->htx.FDFormat = FDCAN_CLASSIC_CAN;
        p_dev->htx.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
        p_dev->htx.MessageMarker = 0;

        // set operation function
        p_dev->ops.enable = can_device_ops_enable;
        p_dev->ops.reinit = can_device_ops_reinit;
        p_dev->ops.buff_send = can_device_ops_buff_send;
        p_dev->ops.update_status = can_device_ops_update_status;
        p_dev->ops.return_status = can_device_ops_return_status;

        // bind the callback function
        if (NULL_PTR == p_func || 0 == p_func)
        {
            p_dev->call_func = NULL_PTR;
        }
        else
        {
            p_dev->call_func = p_func;
        }

        /* call init operation */
        p_dev->ops.enable((void *)p_dev);

    }
}
