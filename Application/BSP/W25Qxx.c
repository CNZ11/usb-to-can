/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 ��ݸ�б�ĩ�Ƽ����޹�˾
 * @file 	: W25Qxx.c
 * ******************************************************************************
 * @brief 	: W25Qxx BSP driver, theoretically support W25QXX series and other pin to pin SPI flash chip
 * @version	: 2.0
 * @date 	: 2024-07-31
 * @note 	: 1. W25QXX ָ����(xx)Mbit��(xx/8) MB������ W25Q128,16 MB,256����
 *            2. W25QXX ��(XX/8) MB ��������Ϊ X �飬ÿ����Ϊ64K�ֽڣ�ÿ�����Ϊ16��������ÿ������Ϊ4K�ֽ�
 *            3. W25QXX ����С������Ԫ��һ������������ζ��ÿ�α������4K �ֽ�
 *            4. ��ˣ�����Ҫ W25Qxx ��һ������4K �Ļ����������Ҫ��оƬӵ�г���4K �� SRAM �Ա�֤���õ�����
 *
 *
 * ******* 1. Improvement *********
 * * 1. Transplant modify dot atomic source code, modify macro definition, encapsulation function.
 * *
 * * 2. Improved macro definition, more standardized naming, easier to port BSP.
 * *
 * * 3. Increased SPI multibyte blocking read functions
 * *
 * ******* 2. Fix the bug *********
 * * 1. fix the bug of :
 * *
 * *
 * ******************************************************************************
 */

/* ---------------------------- user header file ---------------------------- */

#include "W25Qxx.h"

/* ----------------------- global variable definition ----------------------- */

/**
 * @brief:
 */
w25qxx_t g_w25qxx_device = {
    .chip_id = W25Q128,
    .ops = {
        .init = w25qxx_init,
        .read = w25qxx_read,
        .write = w25qxx_write,
        .get_status = w25qxx_read_sr,
    },
};

/* --------------------------- function definition -------------------------- */

/**
 * ******************************************************************************
 * @brief 	: 25qxx SPI ��дһ���ֽ�����
 * @param 	  txdata: Ҫ���͵�����(1�ֽ�)
 * @retval 	: ���յ�������(1�ֽ�)
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
uint8_t w25qxx_spi_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;
    HAL_SPI_TransmitReceive(&W25QXX_SPI_HANDLE, &txdata, &rxdata, 1, W25QXX_SPI_MAX_TIME_OUT);
    return rxdata;
}

/**
 * ******************************************************************************
 * @brief 	: ͨ��spi�Լ����Ե�ַ��ȡw25qxx����
 * @param 	  address  	: ��parameter��
 * @param 	  rxbuff  	: ��parameter��
 * @param 	  length  	: ��parameter��
 * @author 	: chenningzhan
 * @note 	: ������ȡ
 * ******************************************************************************
 */
void w25qxx_read_spi_address(volatile uint32_t address, volatile uint8_t *rxbuff, uint16_t length)
{
    uint8_t spi_buff_tx[length];
    memset(spi_buff_tx, 0xff, length);

    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(FLASH_ReadData);
    w25qxx_send_address(address);
    HAL_SPI_TransmitReceive(&W25QXX_SPI_HANDLE, spi_buff_tx, rxbuff, length, 1000);
    W25QXX_SPI_UNSELECTED();
}

/**
 * ******************************************************************************
 * @brief 	: ����Sector�ı�Ż�ȡ���Ե�ַ,��4096������
 * @param 	  SectorNo  	: ������ַ�ı�ţ�0��4095,һ��Sector��4KB��
 * @retval 	: �����ڵ�ַ
 * @author 	: chenningzhan
 * @note 	: ÿ������4K�ֽڣ�12λ��ַ,�����ڵ�ַ��Χ0x000-0xFFF
 * ******************************************************************************
 */
uint32_t w25qxx_addr_by_sector(uint16_t SectorNo)
{
    if (SectorNo > 4095) // ���ܳ���4095
    {
        SectorNo = 0;
    }
    uint32_t addr = SectorNo;
    addr = addr << 12; // ����12λ,���ڳ���0x1000
    return addr;
}

/**
 * ******************************************************************************
 * @brief 	: 25qxx ��ȡоƬID
 * @retval 	: FLASHоƬID
 * @author 	: chenningzhan
 * @note	: оƬID�б��: .h�ļ�, оƬ�б���
 * ******************************************************************************
 */
uint16_t w25qxx_read_id(void)
{
    uint16_t deviceid;

    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(FLASH_ManufactDeviceID); /* ���Ͷ� ID ���� */
    w25qxx_spi_read_write_byte(0);                      /* д��һ���ֽ� */
    w25qxx_spi_read_write_byte(0);
    w25qxx_spi_read_write_byte(0);
    deviceid = w25qxx_spi_read_write_byte(0xFF) << 8; /* ��ȡ��8λ�ֽ� */
    deviceid |= w25qxx_spi_read_write_byte(0xFF);     /* ��ȡ��8λ�ֽ� */
    W25QXX_SPI_UNSELECTED();

    return deviceid;
}

/**
 * ******************************************************************************
 * @brief 	: w25qxx ��ʼ��
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
void w25qxx_init(void)
{
    uint8_t temp;
    uint16_t flash_type;

    // ȡ��Ƭѡ
    W25QXX_SPI_UNSELECTED();
    // ��ȡFLASH ID
    g_w25qxx_device.chip_id = flash_type = w25qxx_read_id();

    /* SPI FLASHΪW25Q256, ����ʹ��4�ֽڵ�ַģʽ */
    if (flash_type == W25Q256)
    {
        temp = w25qxx_read_sr(3);

        if ((temp & 0X01) == 0)
        {
            w25qxx_write_enable();
            temp |= 1 << 1;
            w25qxx_write_sr(3, temp);

            W25QXX_SPI_SELECTED();
            // ʹ��4�ֽڵ�ַָ��
            w25qxx_spi_read_write_byte(FLASH_Enable4ByteAddr);
            W25QXX_SPI_UNSELECTED();
        }
    }
}

/**
 * ******************************************************************************
 * @brief 	: w25qxx �ȴ�����
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static void w25qxx_wait_busy(void)
{
    /* �ȴ�BUSYλ��� */
    while ((w25qxx_read_sr(1) & 0x01) == 0x01)
        ;
}

/**
 * ******************************************************************************
 * @brief 	: w25qxx ����
 * @author 	: chenningzhan
 * @note 	: None
 * ******************************************************************************
 */
void w25qxx_power_down(void)
{
    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(FLASH_PowerDown);
    W25QXX_SPI_UNSELECTED();
}

/**
 * ******************************************************************************
 * @brief 	: w25qxx ����
 * @author 	: chenningzhan
 * @note 	: None
 * ******************************************************************************
 */
void w25qxx_power_wakup(void)
{
    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(FLASH_ReleasePowerDown);
    W25QXX_SPI_UNSELECTED();
}

/**
 * ******************************************************************************
 * @brief 	: w25qxx дʹ��
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
void w25qxx_write_enable(void)
{
    W25QXX_SPI_SELECTED();
    // ����дʹ��
    w25qxx_spi_read_write_byte(FLASH_WriteEnable);
    W25QXX_SPI_UNSELECTED();
}

/**
 * ******************************************************************************
 * @brief 	: w25qxx ���͵�ַ
 * @param 	  address: Ҫ���͵ĵ�ַ
 * @author 	: chenningzhan
 * @note	: ����оƬ�ͺŵĲ�ͬ, ����24ibt / 32bit��ַ
 * ******************************************************************************
 */
void w25qxx_send_address(uint32_t address)
{
    // ֻ��W25Q256֧��4�ֽڵ�ַģʽ
    if (W25Q256 == g_w25qxx_device.chip_id)
    {
        w25qxx_spi_read_write_byte((uint8_t)((address) >> 24)); /* ���� bit31 ~ bit24 ��ַ */
    }
    w25qxx_spi_read_write_byte((uint8_t)((address) >> 16)); /* ���� bit23 ~ bit16 ��ַ */
    w25qxx_spi_read_write_byte((uint8_t)((address) >> 8));  /* ���� bit15 ~ bit8  ��ַ */
    w25qxx_spi_read_write_byte((uint8_t)address);           /* ���� bit7  ~ bit0  ��ַ */
}

/**
 * ******************************************************************************
 * @brief 	: 25QXX ��ȡ״̬�Ĵ�����25QXXһ����3��״̬�Ĵ���
 * @param 	  regno: ״̬�Ĵ����ţ���Χ:1~3
 * @retval 	: ״̬�Ĵ���ֵ
 * @author 	: chenningzhan
 * @note	: ״̬�Ĵ���1��
 *            BIT7  6   5   4   3   2   1   0
 *            SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 *            SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
 *            TB,BP2,BP1,BP0:FLASH����д��������
 *            WEL:дʹ������
 *            BUSY:æ���λ(1,æ;0,����)
 *            Ĭ��:0x00
 *
 *            ״̬�Ĵ���2��
 *            BIT7  6   5   4   3   2   1   0
 *            SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
 *
 *            ״̬�Ĵ���3��
 *            BIT7      6    5    4   3   2   1   0
 *            HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
 * ******************************************************************************
 */
uint8_t w25qxx_read_sr(uint8_t regno)
{
    uint8_t byte = 0, command = 0;

    switch (regno)
    {
    case 1:
        command = FLASH_ReadStatusReg1; /* ��״̬�Ĵ���1ָ�� */
        break;

    case 2:
        command = FLASH_ReadStatusReg2; /* ��״̬�Ĵ���2ָ�� */
        break;

    case 3:
        command = FLASH_ReadStatusReg3; /* ��״̬�Ĵ���3ָ�� */
        break;

    default:
        command = FLASH_ReadStatusReg1;
        break;
    }

    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(command);     /* ���Ͷ��Ĵ������� */
    byte = w25qxx_spi_read_write_byte(0Xff); /* ��ȡһ���ֽ� */
    W25QXX_SPI_UNSELECTED();

    return byte;
}

/**
 * ******************************************************************************
 * @brief 	: 25QXX д״̬�Ĵ���
 * @param 	  regno	: ״̬�Ĵ����ţ���Χ:1~3
 * @param 	  sr	: Ҫд��״̬�Ĵ�����ֵ
 * @author 	: chenningzhan
 * @note	: �Ĵ���˵����w25qxx_read_sr����˵��
 * ******************************************************************************
 */
void w25qxx_write_sr(uint8_t regno, uint8_t sr)
{
    uint8_t command = 0;

    switch (regno)
    {
    case 1:
        command = FLASH_WriteStatusReg1; /* д״̬�Ĵ���1ָ�� */
        break;

    case 2:
        command = FLASH_WriteStatusReg2; /* д״̬�Ĵ���2ָ�� */
        break;

    case 3:
        command = FLASH_WriteStatusReg3; /* д״̬�Ĵ���3ָ�� */
        break;

    default:
        command = FLASH_WriteStatusReg1;
        break;
    }

    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(command); /* ���Ͷ��Ĵ������� */
    w25qxx_spi_read_write_byte(sr);      /* д��һ���ֽ� */
    W25QXX_SPI_UNSELECTED();
}

/**
 * ******************************************************************************
 * @brief 	: 25QXX ������
 * @param 	  pbuf  	: ���ݴ洢��
 * @param 	  addr  	: ��ʼ��ȡ�ĵ�ַ(���32bit)
 * @param 	  datalen  	: Ҫ��ȡ���ֽ���(���65535)
 * @author 	: chenningzhan
 * @note	: ��ָ����ַ��ʼ��ȡָ�����ȵ�����
 * ******************************************************************************
 */
void w25qxx_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i;

    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(FLASH_ReadData); /* ���Ͷ�ȡ���� */
    w25qxx_send_address(addr);                  /* ���͵�ַ */

    for (i = 0; i < datalen; i++)
    {
        pbuf[i] = w25qxx_spi_read_write_byte(0XFF); /* ѭ����ȡ */
    }

    W25QXX_SPI_UNSELECTED();
}

/**
 * ******************************************************************************
 * @brief 	: 25qxx ��һҳ(0~65535)��д���������256�ֽڵ�����
 * @param 	  pbuf  	: ���ݴ洢��
 * @param 	  addr  	: ��ʼд��ĵ�ַ(���32bit)
 * @param 	  datalen  	: Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
 * @author 	: chenningzhan
 * @note 	: ���256�ֽڵ�����
 * ******************************************************************************
 */
static void w25qxx_write_page(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i;
    /* дʹ�� */
    w25qxx_write_enable();

    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(FLASH_PageProgram);
    w25qxx_send_address(addr);

    for (i = 0; i < datalen; i++)
    {
        w25qxx_spi_read_write_byte(pbuf[i]);
    }

    W25QXX_SPI_UNSELECTED();
    /* �ȴ�д����� */
    w25qxx_wait_busy();
}

/**
 * ******************************************************************************
 * @brief 	: 25qxx �޼���д����
 * @param 	  pbuf  	: ���ݴ洢��
 * @param 	  addr  	: ��ʼд��ĵ�ַ(���32bit)
 * @param 	  datalen  	: Ҫд����ֽ���(���65535)
 * @author 	: chenningzhan
 * @note 	: ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
 * ******************************************************************************
 */
static void w25qxx_write_nocheck(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t pageremain;
    pageremain = 256 - addr % 256; /* ��ҳʣ����ֽ��� */

    if (datalen <= pageremain) /* ������256���ֽ� */
    {
        pageremain = datalen;
    }

    while (1)
    {
        /* ��д���ֽڱ�ҳ��ʣ���ַ���ٵ�ʱ��, һ����д��
         * ��д��ֱ�ӱ�ҳ��ʣ���ַ�����ʱ��, ��д������ҳ��ʣ���ַ, Ȼ�����ʣ�೤�Ƚ��в�ͬ����
         */
        w25qxx_write_page(pbuf, addr, pageremain);

        if (datalen == pageremain) /* д������� */
        {
            break;
        }
        else /* datalen > pageremain */
        {
            pbuf += pageremain;    /* pbufָ���ַƫ��,ǰ���Ѿ�д��pageremain�ֽ� */
            addr += pageremain;    /* д��ַƫ��,ǰ���Ѿ�д��pageremain�ֽ� */
            datalen -= pageremain; /* д���ܳ��ȼ�ȥ�Ѿ�д���˵��ֽ��� */

            if (datalen > 256) /* ʣ�����ݻ�����һҳ,����һ��дһҳ */
            {
                pageremain = 256; /* һ�ο���д��256���ֽ� */
            }
            else /* ʣ������С��һҳ,����һ��д�� */
            {
                pageremain = datalen; /* ����256���ֽ��� */
            }
        }
    }
}

/**
 * ******************************************************************************
 * @brief 	: 25QXX д����
 * @param 	  pbuf  	: ���ݴ洢��
 * @param 	  addr  	: ��ʼд��ĵ�ַ(���32bit)
 * @param 	  datalen  	: Ҫд����ֽ���(���65535)
 * @author 	: chenningzhan
 * @note 	: ��ָ����ַ��ʼд��ָ�����ȵ����� , �ú�������������!
 * ******************************************************************************
 */
void w25qxx_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t norflash_buf[4096];

    secpos = addr / 4096;      /* ������ַ */
    secoff = addr % 4096;      /* �������ڵ�ƫ�� */
    secremain = 4096 - secoff; /* ����ʣ��ռ��С */

    if (datalen <= secremain)
    {
        secremain = datalen; /* ������4096���ֽ� */
    }

    while (1)
    {
        w25qxx_read(norflash_buf, secpos * 4096, 4096); /* ������������������ */

        for (i = 0; i < secremain; i++) /* У������ */
        {
            if (norflash_buf[secoff + i] != 0XFF)
            {
                break; /* ��Ҫ����, ֱ���˳�forѭ�� */
            }
        }

        if (i < secremain) /* ��Ҫ���� */
        {
            w25qxx_erase_sector(secpos); /* ����������� */

            for (i = 0; i < secremain; i++) /* ���� */
            {
                norflash_buf[i + secoff] = pbuf[i];
            }

            w25qxx_write_nocheck(norflash_buf, secpos * 4096, 4096); /* д���������� */
        }
        else /* д�Ѿ������˵�,ֱ��д������ʣ������. */
        {
            w25qxx_write_nocheck(pbuf, addr, secremain); /* ֱ��д���� */
        }

        if (datalen == secremain)
        {
            break; /* д������� */
        }
        else /* д��δ���� */
        {
            secpos++;   /* ������ַ��1 */
            secoff = 0; /* ƫ��λ��Ϊ0 */

            pbuf += secremain;    /* ָ��ƫ�� */
            addr += secremain;    /* д��ַƫ�� */
            datalen -= secremain; /* �ֽ����ݼ� */

            if (datalen > 4096)
            {
                secremain = 4096; /* ��һ����������д���� */
            }
            else
            {
                secremain = datalen; /* ��һ����������д���� */
            }
        }
    }
}

/**
 * ******************************************************************************
 * @brief 	: ����һ������
 * @param 	  saddr  	: ������ַ ����ʵ����������
 * @author 	: chenningzhan
 * @note 	: ע��,������������ַ,�����ֽڵ�ַ!!
 * ����һ������������ʱ��:150ms
 * ******************************************************************************
 */
void w25qxx_erase_sector(uint32_t saddr)
{
    saddr *= 4096;
    w25qxx_write_enable(); /* дʹ�� */
    w25qxx_wait_busy();    /* �ȴ����� */

    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(FLASH_SectorErase); /* ����дҳ���� */
    w25qxx_send_address(saddr);                    /* ���͵�ַ */
    W25QXX_SPI_UNSELECTED();
    w25qxx_wait_busy(); /* �ȴ������������ */
}

/**
 * ******************************************************************************
 * @brief 	: ��������оƬ
 * @author 	: chenningzhan
 * @note 	:  �ȴ�ʱ�䳬��...
 * ******************************************************************************
 */
void w25qxx_erase_chip(void)
{
    w25qxx_write_enable(); /* дʹ�� */
    w25qxx_wait_busy();    /* �ȴ����� */
    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(FLASH_ChipErase); /* ���Ͷ��Ĵ������� */
    W25QXX_SPI_UNSELECTED();
    w25qxx_wait_busy(); /* �ȴ�оƬ�������� */
}