/**
 * W25Q128 оƬ������ 16M�ֽڣ�24λ��ַ��
 * ��Ϊ256��Block��ÿ��Block 64K�ֽ�
 * һ��Block�ַ�Ϊ16��Sector����4096��Sector��ÿ��Sector 4K�ֽ�
 * һ��Sector�ַ�Ϊ16��Page����65536��Page��ÿ��Page 256�ֽ�
 * д���ݲ����Ļ�����Ԫ��Page��һ������д��������ܳ���һ��Page�ķ�Χ��д��Page�����ǲ������ġ�
 */

#ifndef __W25QXX_H__
#define __W25QXX_H__

/* ---------------------------- user header file ---------------------------- */
#include "spi.h"
#include "user_bsp.h"

/* ---------------------------- macro definition ---------------------------- */

/**
 * @brief The macro definition of W25QXX chip and MCU hardware resource
 */
#define W25QXX_SPI_HANDLE hspi1
#define W25QXX_SPI_CS_PORT FLASH_CS_GPIO_Port
#define W25QXX_SPI_CS_PIN FLASH_CS_Pin
#define W25QXX_SPI_CLK_PORT FLASH_CLK_GPIO_Port
#define W25QXX_SPI_CLK_PIN FLASH_CLK_Pin
#define W25QXX_SPI_MOSI_PORT FLASH_DI_GPIO_Port
#define W25QXX_SPI_MOSI_PIN FLASH_DI_Pin
#define W25QXX_SPI_MISO_PORT FLASH_DO_GPIO_Port
#define W25QXX_SPI_MISO_PIN FLASH_DO_Pin

#define W25QXX_SPI_MAX_TIME_OUT 1000

#define W25Q128_SIZE (16 * 1024 * 1024)

#define W25Q128_BLOCK_SIZE (64 * 1024)
#define W25Q128_BLOCK_COUNT (W25Q128_SIZE / W25Q128_BLOCK_SIZE)

#define W25Q128_SECTOR_SIZE (4 * 1024)
#define W25Q128_SECTOR_COUNT (16 * W25Q128_BLOCK_COUNT)

#define W25Q128_PAGE_SIZE 256
#define W25Q128_PAGE_COUNT (16 * W25Q128_SECTOR_COUNT)

#define W25QXX_SPI_SELECTED()                                                \
    do                                                                       \
    {                                                                        \
        HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET); \
    } while (0)
#define W25QXX_SPI_UNSELECTED()                                            \
    do                                                                     \
    {                                                                      \
        HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET); \
    } while (0)

typedef struct
{
    void (*init)(void);
    void (*read)(uint8_t *pbuf, uint32_t addr, uint16_t datalen);
    void (*write)(uint8_t *pbuf, uint32_t addr, uint16_t datalen);
    uint8_t (*get_status)(uint8_t regno);
} w25qxx_ops_t;

typedef struct
{
    uint16_t chip_id;
    w25qxx_ops_t ops;
} w25qxx_t;

extern w25qxx_t g_w25qxx_device;

/**
 * @brief SPI FLASH оƬ�ͺ��б�
 *
 * 0xEF17,��ʾоƬ�ͺ�ΪW25Q128, Winbond
 * 0xC817����ʾоƬ�ͺ�ΪGD25Q128,ELM
 * x1C17����ʾоƬ�ͺ�ΪEN25Q128,̨��EON
 * 0xA117����ʾоƬ�ͺ�ΪFM25Q128������΢����
 * 0x2018����ʾоƬ�ͺ�ΪN25Q128������
 * 0x2017����ʾоƬ�ͺ�ΪXM25QH128���人��о
 *
 */
#define W25Q80 0XEF13
#define W25Q16 0XEF14
#define W25Q32 0XEF15
#define W25Q64 0XEF16
#define W25Q128 0XEF17
#define W25Q256 0XEF18
#define BY25Q64 0X6816
#define BY25Q128 0X6817
#define NM25Q64 0X5216
#define NM25Q128 0X5217

/**
 * @brief SPI FLASH оƬ����
 */
#define FLASH_WriteEnable 0x06
#define FLASH_WriteDisable 0x04
#define FLASH_ReadStatusReg1 0x05
#define FLASH_ReadStatusReg2 0x35
#define FLASH_ReadStatusReg3 0x15
#define FLASH_WriteStatusReg1 0x01
#define FLASH_WriteStatusReg2 0x31
#define FLASH_WriteStatusReg3 0x11
#define FLASH_ReadData 0x03
#define FLASH_FastReadData 0x0B
#define FLASH_FastReadDual 0x3B
#define FLASH_FastReadQuad 0xEB
#define FLASH_PageProgram 0x02
#define FLASH_PageProgramQuad 0x32
#define FLASH_BlockErase 0xD8
#define FLASH_SectorErase 0x20
#define FLASH_ChipErase 0xC7
#define FLASH_PowerDown 0xB9
#define FLASH_ReleasePowerDown 0xAB
#define FLASH_DeviceID 0xAB
#define FLASH_ManufactDeviceID 0x90
#define FLASH_JedecDeviceID 0x9F
#define FLASH_Enable4ByteAddr 0xB7
#define FLASH_Exit4ByteAddr 0xE9
#define FLASH_SetReadParam 0xC0
#define FLASH_EnterQPIMode 0x38
#define FLASH_ExitQPIMode 0xFF

// ����
uint32_t w25qxx_addr_by_sector(uint16_t SectorNo);
void w25qxx_read_spi_address(volatile uint32_t address, volatile uint8_t *rxbuff, uint16_t length);
uint8_t w25qxx_spi_read_write_byte(uint8_t txdata);
void w25qxx_init(void);
uint16_t w25qxx_read_id(void);
void w25qxx_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen);
void w25qxx_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen);

// ������
void w25qxx_send_address(uint32_t address);
void w25qxx_power_down(void);
void w25qxx_power_wakup(void);
uint8_t w25qxx_read_sr(uint8_t regno);
void w25qxx_write_sr(uint8_t regno, uint8_t sr);
void w25qxx_write_enable(void);

void w25qxx_erase_chip(void);
void w25qxx_erase_sector(uint32_t saddr);

#endif