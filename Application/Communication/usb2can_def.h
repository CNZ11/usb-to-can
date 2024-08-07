/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: usb2can_def.h
 * ******************************************************************************
 * @brief 	: The protocol structure of USB to can module is defined according to the host computer protocol
 * @version	: 1.0
 * @date 	: 2024-07-24
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
 * * 1. #include 'filesname.h'
 * * 2. change the macro and enum in 'filesname.h':
 * * 3. change the resources in 'filesname.c':
 * * 4. Initialize,Use a pointer to get an action handle, and use an action function
 * *
 * ******************************************************************************
 */
#ifndef _USB2CAN_DEF_H_
#define _USB2CAN_DEF_H_

/* ---------------------------- user header file ---------------------------- */
#include "main.h"

/* ---------------------------- macro definition ---------------------------- */
#define USB2CAN_CAN_DATA_LEN 8   // Length of CAN data in bytes.
#define USB2CAN_PC2USBCDC_LEN 30 // Length of data packet from PC to USB CDC.
#define USB2CAN_USBCDC2PC_LEN 16 // Length of data packet from USB CDC to PC.

// can sends data commands
#define PC2USBCDC_TRANSMIT_FREAM_HEADER_1 0X55
#define PC2USBCDC_TRANSMIT_FREAM_HEADER_2 0XAA
#define PC2USBCDC_TRANSMIT_FREAM_LEN 0X1E
// can stop sending (continuous sending)
#define PC2USBCDC_STOP_FREAM_HEADER_1 0X55
#define PC2USBCDC_STOP_FREAM_HEADER_2 0X03
#define PC2USBCDC_STOP_FREAM_END_1 0XAA
#define PC2USBCDC_STOP_FREAM_END_2 0X55
// set the CAN baud rate command
#define PC2USBCDC_BAUDRATE_FREAM_HEADER_1 0X55
#define PC2USBCDC_BAUDRATE_FREAM_HEADER_2 0X05
#define PC2USBCDC_BAUDRATE_FREAM_END_1 0XAA
#define PC2USBCDC_BAUDRATE_FREAM_END_2 0X55
// set the CAN baud rate command
#define PC2USBCDC_HEARTBEAT_FREAM_HEADER_1 0X55
#define PC2USBCDC_HEARTBEAT_FREAM_HEADER_2 0X04
#define PC2USBCDC_HEARTBEAT_FREAM_END_1 0XAA
#define PC2USBCDC_HEARTBEAT_FREAM_END_2 0X55
// usbhid return the can data to pc
#define USBCDC2PC_FREAM_HEADER 0XAA
#define USBCDC2PC_FREAM_END 0X55

/* --------------------------- struct definitions --------------------------- */

/**
 * @brief Enumeration for PC to USB HID command types
 */
typedef enum
{
    PC2USBCDC_CMD_TRANSMIT = 0x01,  // Send CAN data frame
    PC2USBCDC_CMD_HEARTBEAT = 0x02, // PC handshake command with the device
    PC2USBCDC_CMD_QUIET = 0x03,     // Non-feedback CAN forward command
} pc2usbcdc_fream_transmit_cmd_t;

/**
 * @brief Structure for PC to USB HID data packet, 30bytes
 * @note
 * eg: 55 aa 1e 01 01 00 00 00 0a 00 00 00 00 00 00 00 00 00 08 00 00 12 23 34 45 56 67 78 89 88
 */
typedef struct
{
    uint8_t fream_header_1;                 // Frame header byte 1
    uint8_t fream_header_2;                 // Frame header byte 2
    uint8_t fream_len;                      // Length of the frame
    pc2usbcdc_fream_transmit_cmd_t cmd;     // Command type
    uint32_t repeat_count;                  // Repeat count for sendings
    uint32_t repeat_interval;               // Interval time for repeats (ms)
    uint32_t can_id_type;                   // CAN ID type (0 for standard, 1 for extended)
    uint32_t can_identifier;                // CAN Identifier
    uint32_t can_frame_type;                // CAN Frame type (0 for data, 1 for remote)
    uint8_t can_inc_id;                     // CAN ID increment flag
    uint8_t can_inc_data;                   // CAN Data increment flag
    uint8_t can_data_len;                   // Length of CAN data
    uint8_t can_data[USB2CAN_CAN_DATA_LEN]; // CAN data array
    uint8_t fream_end_crc;                  // CRC8 for end of frame
} pc2usbcdc_fream_transmit_t;

/**
 * @brief Enumeration for PC to set can baud rate
 */
typedef enum
{
    CAN_BAUDRATE_1M = 0,
    CAN_BAUDRATE_500K = 3,
    CAN_BAUDRATE_250K = 5,
    CAN_BAUDRATE_200K = 6,
    CAN_BAUDRATE_125K = 7,
    CAN_BAUDRATE_100K = 8,
    CAN_BAUDRATE_TYPE,
} pc2usbcdc_fream_can_baudrate_index_t;

/**
 * @brief Structure for PC to set can baud rate , 5bytes
 * @note
 * eg: 0x55 0x05 0x00 xAA 0x55
 */
typedef struct
{
    uint8_t fream_header_1;
    uint8_t fream_header_2;
    pc2usbcdc_fream_can_baudrate_index_t baudrate_index;
    uint8_t fream_end_1st;
    uint8_t fream_end_2nd;
} pc2usbcdc_fream_can_baudrate_t;

/**
 * @brief Structure for PC to set stop can transmission, 4bytes
 * @note
 * eg: 0x55 0x03 0xAA 0x55
 */
typedef struct
{
    uint8_t fream_header_1;
    uint8_t fream_header_2;
    uint8_t fream_end_1st;
    uint8_t fream_end_2nd;
} pc2usbcdc_fream_set_stop_t;

/**
 * @brief Enumeration for CAN to PC command types
 */
typedef enum
{
    CAN_TO_PC_CMD_HEARTBEAT = 0x00,     // heartbeat
    CAN_TO_PC_CMD_REC_FAILED = 0x01,    // receiving failure
    CAN_TO_PC_CMD_REC_SUCCEED = 0x11,   // receiving success
    CAN_TO_PC_CMD_SEND_FAILED = 0x02,   // sending failure
    CAN_TO_PC_CMD_SEND_SUCCEED = 0x12,  // sending success
    CAN_TO_PC_CMD_SETUP_FAILED = 0x03,  // setup buad rate failure
    CAN_TO_PC_CMD_SETUP_SUCCEED = 0x13, // setup buad rate success
} usbcdc2pc_fream_can_cmd_t;

/**
 * @brief Structure for CAN data returned to PC, 16bytes
 * @note  CMD 00 the rest of the CAN data is invalid
 * eg: AA 11 08 0x2FF 00 01 02 03 04 05 06 07 55
 */
typedef struct
{
    uint8_t fream_header;                   // freamHeader
    usbcdc2pc_fream_can_cmd_t cmd;          // CAN command
    uint8_t can_data_len : 6;               // Data Length
    uint8_t can_ide : 1;                    // IDE: 0, standard frame; 1, extended frame
    uint8_t can_rtr : 1;                    // RTR: 0, data frame; 1, remote frame
    uint32_t can_id;                        // can ID
    uint8_t can_data[USB2CAN_CAN_DATA_LEN]; // can data array 8 byte
    uint8_t fream_end;                      // end of fream 0x55
} usbcdc2pc_fream_can_t;

/* -------------------------- function declaration -------------------------- */

#endif