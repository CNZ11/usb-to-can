/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: task_usb2can.c
 * ******************************************************************************
 * @brief 	: This file contains the implementation of the USB to CAN communication task.
 * @version	: 1.0
 * @date 	: 2024-07-24
 * @note 	:
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
 * * 1. #include 'task_usb2can.h'
 * * 2. change the macro and enum in 'task_usb2can.h':
 * * 3. change the resources in 'task_usb2can.c':
 * * 4. Initialize,Use a pointer to get an action handle, and use an action function
 * *
 * ******************************************************************************
 */

/* ---------------------------- user header file ---------------------------- */
#include "task_usb2can.h"
#include "device_can.h"
#include "device_usb2can.h"
#include "device_usbcdc.h"
#include "ops_byte_bit.h"

/* --------------------------- function definition -------------------------- */

/**
 * ******************************************************************************
 * @brief 	: Callback function for the CAN receive FIFO 0 interrupt.
 * @param 	  hfdcan  	: Pointer to the FDCAN handle
 * @param 	  RxFifo0ITs  	: Status of the RX FIFO0 interrupt.
 * @author 	: chenningzhan
 * @note 	: None
 * ******************************************************************************
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    can_device_t *p_dev_can = can_device_get_pointer(CAN_ID_FD1);
    usb2can_device_t *p_dev_usb2can = (usb2can_device_t *)usb2can_device_get_pointer();
    usbcdc_device_t *p_dev_usbcdc = (usbcdc_device_t *)usbcdc_device_get_pointer();

    // Check for new message interrupt
    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
    {
        if (hfdcan->Instance == p_dev_can->hfdcan->Instance)
        {
            /* Retrieve Rx messages from RX FIFO0 */
            if (HAL_FDCAN_GetRxMessage(p_dev_can->hfdcan, FDCAN_RX_FIFO0, &p_dev_can->hrx, p_dev_can->buff_rx) != HAL_OK)
            {
                Error_Handler();
                p_dev_can->ops.update_status((void *)p_dev_can, CAN_STATUS_RECEIVE_FAILED);
            }
            else
            {
                p_dev_can->ops.update_status((void *)p_dev_can, CAN_STATUS_RECEIVE_SUCCESS);
            }
        }
    }
    // 1.1 Update the USB2CAN device's action to send CAN data to the PC.
    p_dev_usb2can->ops.update_action((void *)p_dev_usb2can, USB2CAN_ACTION_FB_TX2PC_CAN);

    // 1.2 call the USBCDC device's callback function to feedback data to the pc
    p_dev_usbcdc->call_func((void *)p_dev_usbcdc);

    // 2. Reset status to idle
    p_dev_usbcdc->ops.update_status((void *)p_dev_usbcdc, USBCDC_STATUS_IDLE);
    p_dev_can->ops.update_status((void *)p_dev_can, CAN_STATUS_IDLE);
    p_dev_usb2can->ops.update_action((void *)p_dev_usb2can, USB2CAN_ACTION_IDLE);
}

/**
 * ******************************************************************************
 * @brief 	: Callback function for the usb device
 * @param 	  p_self  	: Pointer to the device structure.
 * @author 	: chenningzhan
 * @note 	: None
 * ******************************************************************************
 */
void usbcdc_device_callback(void *p_self)
{
    /* Gets the device pointer */
    usbcdc_device_t *p_dev_usbcdc = (usbcdc_device_t *)p_self;
    usb2can_device_t *p_dev_usb2can = (usb2can_device_t *)usb2can_device_get_pointer();
    can_device_t *p_dev_can = (can_device_t *)can_device_get_pointer(CAN_ID_FD1);

    /* Empty the send data buffer every time the structure data is updated */
    memset(p_dev_usbcdc->buff_tx, 0, ARRAY_NUM(p_dev_usbcdc->buff_tx));

    /*
     * Defines three 1-byte data for bit-combining operations on the third data status code
     * of the sending buffer array
     */
    uint8_t usbcdc2pc_data2_fream = 0x00;
    uint8_t canDataLen = USB2CAN_CAN_DATA_LEN; // can data length, 6 bits
    uint8_t canIde = 0;                        // identifier extension bit, 1 bit
    uint8_t canRtr = 0;                        // remote transfer request bit, 1 bit

    /* According to the behavior of USB to can device, update the corresponding structure data */
    switch (p_dev_usb2can->ops.return_action((void *)p_dev_usb2can))
    {
    /* feedback heartbeat data */
    case USB2CAN_ACTION_FB_TX2PC_HEARTBEAT:
        // If it is a heartbeat packet, NO can data is required
        p_dev_usbcdc->buff_tx[1] = CAN_TO_PC_CMD_HEARTBEAT;
        break;

    /* feedback the message received by CAN bus to PC */
    case USB2CAN_ACTION_FB_TX2PC_CAN:
        switch (p_dev_can->ops.return_status((void *)p_dev_can))
        {
        case CAN_STATUS_RECEIVE_SUCCESS:
            // packet cmd
            p_dev_usbcdc->buff_tx[1] = CAN_TO_PC_CMD_REC_SUCCEED;

            // identifier extension bit and remote transfer request bit
            canIde = (p_dev_can->hrx.IdType == FDCAN_EXTENDED_ID) ? 1 : 0;
            canRtr = (p_dev_can->hrx.RxFrameType == FDCAN_REMOTE_FRAME) ? 1 : 0;

            // can identifier ,is big endian
            split_uint32_to_bytes_big_endian(p_dev_can->hrx.Identifier,
                                             &p_dev_usbcdc->buff_tx[6],
                                             &p_dev_usbcdc->buff_tx[5],
                                             &p_dev_usbcdc->buff_tx[4],
                                             &p_dev_usbcdc->buff_tx[3]);
            // can data 8 bytes
            for (uint8_t i = 0, j = 7; i < USB2CAN_CAN_DATA_LEN; i++, j++)
            {
                p_dev_usbcdc->buff_tx[j] = p_dev_can->buff_rx[i];
            }
            break;

        case CAN_STATUS_SENDOUT_SUCCESS:
            // packet cmd
            p_dev_usbcdc->buff_tx[1] = CAN_TO_PC_CMD_SEND_SUCCEED;

            // identifier extension bit and remote transfer request bit
            canIde = (p_dev_can->htx.IdType == FDCAN_EXTENDED_ID) ? 1 : 0;
            canRtr = (p_dev_can->htx.TxFrameType == FDCAN_REMOTE_FRAME) ? 1 : 0;

            // can identifier ,is big endian
            split_uint32_to_bytes_big_endian(p_dev_can->htx.Identifier,
                                             &p_dev_usbcdc->buff_tx[6],
                                             &p_dev_usbcdc->buff_tx[5],
                                             &p_dev_usbcdc->buff_tx[4],
                                             &p_dev_usbcdc->buff_tx[3]);
            // can data 8 bytes
            for (uint8_t i = 0, j = 7; i < USB2CAN_CAN_DATA_LEN; i++, j++)
            {
                p_dev_usbcdc->buff_tx[j] = p_dev_can->buff_tx[i];
            }
            break;
        case CAN_STATUS_SETUP_SUCCESS:
            p_dev_usbcdc->buff_tx[1] = CAN_TO_PC_CMD_SETUP_SUCCEED;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    /* The third data of the array : status code, bit combination operation */
    // clear existing fields to ensure they do not affect the results
    usbcdc2pc_data2_fream &= ~((1 << 6) | (1 << 7));
    usbcdc2pc_data2_fream |= (canDataLen & 0x3F) |
                             ((canIde & 0x01) << 6) |
                             ((canRtr & 0x01) << 7);

    p_dev_usbcdc->buff_tx[0] = USBCDC2PC_FREAM_HEADER;                           // fream header
    p_dev_usbcdc->buff_tx[2] = usbcdc2pc_data2_fream;                            // cmd
    p_dev_usbcdc->buff_tx[USBCDC_DEVICE_BUFF_SIZE_TX - 1] = USBCDC2PC_FREAM_END; // end of fream

    /* send data to usbcdc */
    p_dev_usbcdc->ops.buff_send((void *)p_dev_usbcdc);
}

/**
 * ******************************************************************************
 * @brief 	: Callback function for the can device
 * @param 	  p_self  	: Pointer to the device structure.
 * @author 	: chenningzhan
 * @note 	: None
 * ******************************************************************************
 */
void can_device_callback(void *p_self)
{
    can_device_t *p_dev_can = (can_device_t *)p_self;
    usb2can_device_t *p_dev_usb2can = (usb2can_device_t *)usb2can_device_get_pointer();
    usbcdc_device_t *p_dev_usbcdc = (usbcdc_device_t *)usbcdc_device_get_pointer();

    switch (p_dev_usb2can->ops.return_action((void *)p_dev_usb2can))
    {
    case USB2CAN_ACTION_OPS_BAUDRATE:
        p_dev_can->params.baudrate = p_dev_usbcdc->buff_rx[2];
        p_dev_can->ops.reinit((void *)p_dev_can);
        break;

    case USB2CAN_ACTION_OPS_STOP:
        break;

    case USB2CAN_ACTION_OPS_TX2CAN:
        // can idtype
        p_dev_can->htx.IdType = (p_dev_usbcdc->buff_rx[12] == 1) ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;

        // can identifier
        p_dev_can->htx.Identifier = combine_bytes_to_uint32_big_endian(p_dev_usbcdc->buff_rx[16],
                                                                       p_dev_usbcdc->buff_rx[15],
                                                                       p_dev_usbcdc->buff_rx[14],
                                                                       p_dev_usbcdc->buff_rx[13]);
        // can tx frame type
        p_dev_can->htx.TxFrameType = (p_dev_usbcdc->buff_rx[17] == 1) ? FDCAN_REMOTE_FRAME : FDCAN_DATA_FRAME;

        // can dtat 8byte
        for (uint8_t i = 21, j = 0; i <= 28; i++, j++)
        {
            p_dev_can->buff_tx[j] = p_dev_usbcdc->buff_rx[i];
        }
        // end of parse packet
        // send the data to the can bus
        p_dev_can->ops.buff_send((void *)p_dev_can);
        break;
    default:
        break;
    }
}

/**
 * ******************************************************************************
 * @brief 	: Callback function for the usb to can device
 * @param 	  p_self  	: a pointer to get the device object
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
void usb2can_device_callback(void *p_self)
{
    usb2can_device_t *p_dev_usb2can = (usb2can_device_t *)p_self;
    usbcdc_device_t *p_dev_usbcdc = (usbcdc_device_t *)usbcdc_device_get_pointer();

    switch (p_dev_usb2can->ops.return_action((void *)p_dev_usb2can))
    {
    case USB2CAN_ACTION_PARSE:
        // enable transmit the packet to the can bus
        if (p_dev_usbcdc->buff_rx[0] == PC2USBCDC_TRANSMIT_FREAM_HEADER_1 &&
            p_dev_usbcdc->buff_rx[1] == PC2USBCDC_TRANSMIT_FREAM_HEADER_2 &&
            p_dev_usbcdc->buff_rx[2] == PC2USBCDC_TRANSMIT_FREAM_LEN)
        {
            p_dev_usb2can->ops.update_action((void *)p_dev_usb2can, USB2CAN_ACTION_OPS_TX2CAN);
        }
        // stop transmit the packet to the can bus
        else if (p_dev_usbcdc->buff_rx[0] == PC2USBCDC_STOP_FREAM_HEADER_1 &&
                 p_dev_usbcdc->buff_rx[1] == PC2USBCDC_STOP_FREAM_HEADER_2 &&
                 p_dev_usbcdc->buff_rx[2] == PC2USBCDC_STOP_FREAM_END_1 &&
                 p_dev_usbcdc->buff_rx[3] == PC2USBCDC_STOP_FREAM_END_2)
        {
            p_dev_usb2can->ops.update_action((void *)p_dev_usb2can, USB2CAN_ACTION_OPS_STOP);
        }
        // fix the can baud rate
        else if (p_dev_usbcdc->buff_rx[0] == PC2USBCDC_BAUDRATE_FREAM_HEADER_1 &&
                 p_dev_usbcdc->buff_rx[1] == PC2USBCDC_BAUDRATE_FREAM_HEADER_2 &&
                 p_dev_usbcdc->buff_rx[3] == PC2USBCDC_BAUDRATE_FREAM_END_1 &&
                 p_dev_usbcdc->buff_rx[4] == PC2USBCDC_BAUDRATE_FREAM_END_2)

        {
            p_dev_usb2can->ops.update_action((void *)p_dev_usb2can, USB2CAN_ACTION_OPS_BAUDRATE);
        }
        else if (p_dev_usbcdc->buff_rx[0] == PC2USBCDC_HEARTBEAT_FREAM_HEADER_1 &&
                 p_dev_usbcdc->buff_rx[1] == PC2USBCDC_HEARTBEAT_FREAM_HEADER_2 &&
                 p_dev_usbcdc->buff_rx[2] == PC2USBCDC_HEARTBEAT_FREAM_END_1 &&
                 p_dev_usbcdc->buff_rx[3] == PC2USBCDC_HEARTBEAT_FREAM_END_2)
        // return the packet to pc heartbeat
        {
            p_dev_usb2can->ops.update_action((void *)p_dev_usb2can, USB2CAN_ACTION_FB_TX2PC_HEARTBEAT);
        }
        else
        {
            p_dev_usb2can->ops.update_action((void *)p_dev_usb2can, USB2CAN_ACTION_UNKNOWN);
        }
        // end of case USB2CAN_ACTION_PARSE
        break;
    default:
        break;
    }
}

/**
 * ******************************************************************************
 * @brief 	: Entry point for the USB2CAN task.
 * @param 	  p_self  	: a pointer to get the task object
 * @author 	: chenningzhan
 * @note 	: None
 * ******************************************************************************
 */
void mytask_usb2can_entry(void *p_self)
{
    // Declare static pointers devices
    schedule_t *task = (schedule_t *)p_self;
    static can_device_t *p_dev_can = (void *)NULL;
    static usbcdc_device_t *p_dev_usbcdc = (void *)NULL;
    static usb2can_device_t *p_dev_usb2can = (void *)NULL;

    // the task initialization
    if (0 == task->params.count)
    {
        /* Register the device callback */
        can_device_register(can_device_callback);
        usbcdc_device_register(usbcdc_device_callback);
        usb2can_device_register(usb2can_device_callback);

        p_dev_can = can_device_get_pointer(CAN_ID_FD1);
        p_dev_usbcdc = usbcdc_device_get_pointer();
        p_dev_usb2can = usb2can_device_get_pointer();

        // check the pointer
        if ((void *)NULL == p_dev_can || (void *)NULL == p_dev_usbcdc || (void *)NULL == p_dev_usb2can)
        {
            LOG_ERROR("get device pointer failed !");
            return;
        }
        else
        {
            LOG_INFO("find device success");
            p_dev_can->ops.enable((void *)p_dev_can);
        }
    }
    // the task starts a loop
    else
    {
        //
    }
}
