/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)  2021-2024 东莞市本末科技有限公司
 * @file 	: W25Qxx.c
 * ******************************************************************************
 * @brief 	: W25Qxx BSP driver, theoretically support W25QXX series and other pin to pin SPI flash chip
 * @version	: 2.0
 * @date 	: 2024-07-31
 * @note 	: 1. W25QXX 指的是(xx)Mbit，(xx/8) MB，例如 W25Q128,16 MB,256个块
 *            2. W25QXX 将(XX/8) MB 的容量分为 X 块，每个块为64K字节，每个块分为16个扇区，每个扇区为4K字节
 *            3. W25QXX 的最小擦除单元是一个扇区，这意味着每次必须擦除4K 字节
 *            4. 因此，这需要 W25Qxx 打开一个至少4K 的缓冲区，这就要求芯片拥有超过4K 的 SRAM 以保证良好的运行
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
 * @brief 	: 25qxx SPI 读写一个字节数据
 * @param 	  txdata: 要发送的数据(1字节)
 * @retval 	: 接收到的数据(1字节)
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
 * @brief 	: 通过spi以及绝对地址读取w25qxx数据
 * @param 	  address  	: 【parameter】
 * @param 	  rxbuff  	: 【parameter】
 * @param 	  length  	: 【parameter】
 * @author 	: chenningzhan
 * @note 	: 阻塞读取
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
 * @brief 	: 根据Sector的编号获取绝对地址,共4096个扇区
 * @param 	  SectorNo  	: 扇区地址的编号（0至4095,一个Sector是4KB）
 * @retval 	: 扇区内地址
 * @author 	: chenningzhan
 * @note 	: 每个扇区4K字节，12位地址,扇区内地址范围0x000-0xFFF
 * ******************************************************************************
 */
uint32_t w25qxx_addr_by_sector(uint16_t SectorNo)
{
    if (SectorNo > 4095) // 不能超过4095
    {
        SectorNo = 0;
    }
    uint32_t addr = SectorNo;
    addr = addr << 12; // 左移12位,等于乘以0x1000
    return addr;
}

/**
 * ******************************************************************************
 * @brief 	: 25qxx 读取芯片ID
 * @retval 	: FLASH芯片ID
 * @author 	: chenningzhan
 * @note	: 芯片ID列表见: .h文件, 芯片列表部分
 * ******************************************************************************
 */
uint16_t w25qxx_read_id(void)
{
    uint16_t deviceid;

    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(FLASH_ManufactDeviceID); /* 发送读 ID 命令 */
    w25qxx_spi_read_write_byte(0);                      /* 写入一个字节 */
    w25qxx_spi_read_write_byte(0);
    w25qxx_spi_read_write_byte(0);
    deviceid = w25qxx_spi_read_write_byte(0xFF) << 8; /* 读取高8位字节 */
    deviceid |= w25qxx_spi_read_write_byte(0xFF);     /* 读取低8位字节 */
    W25QXX_SPI_UNSELECTED();

    return deviceid;
}

/**
 * ******************************************************************************
 * @brief 	: w25qxx 初始化
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
void w25qxx_init(void)
{
    uint8_t temp;
    uint16_t flash_type;

    // 取消片选
    W25QXX_SPI_UNSELECTED();
    // 读取FLASH ID
    g_w25qxx_device.chip_id = flash_type = w25qxx_read_id();

    /* SPI FLASH为W25Q256, 必须使能4字节地址模式 */
    if (flash_type == W25Q256)
    {
        temp = w25qxx_read_sr(3);

        if ((temp & 0X01) == 0)
        {
            w25qxx_write_enable();
            temp |= 1 << 1;
            w25qxx_write_sr(3, temp);

            W25QXX_SPI_SELECTED();
            // 使能4字节地址指令
            w25qxx_spi_read_write_byte(FLASH_Enable4ByteAddr);
            W25QXX_SPI_UNSELECTED();
        }
    }
}

/**
 * ******************************************************************************
 * @brief 	: w25qxx 等待空闲
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
static void w25qxx_wait_busy(void)
{
    /* 等待BUSY位清空 */
    while ((w25qxx_read_sr(1) & 0x01) == 0x01)
        ;
}

/**
 * ******************************************************************************
 * @brief 	: w25qxx 休眠
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
 * @brief 	: w25qxx 唤醒
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
 * @brief 	: w25qxx 写使能
 * @author 	: chenningzhan
 * @note	: None
 * ******************************************************************************
 */
void w25qxx_write_enable(void)
{
    W25QXX_SPI_SELECTED();
    // 发送写使能
    w25qxx_spi_read_write_byte(FLASH_WriteEnable);
    W25QXX_SPI_UNSELECTED();
}

/**
 * ******************************************************************************
 * @brief 	: w25qxx 发送地址
 * @param 	  address: 要发送的地址
 * @author 	: chenningzhan
 * @note	: 根据芯片型号的不同, 发送24ibt / 32bit地址
 * ******************************************************************************
 */
void w25qxx_send_address(uint32_t address)
{
    // 只有W25Q256支持4字节地址模式
    if (W25Q256 == g_w25qxx_device.chip_id)
    {
        w25qxx_spi_read_write_byte((uint8_t)((address) >> 24)); /* 发送 bit31 ~ bit24 地址 */
    }
    w25qxx_spi_read_write_byte((uint8_t)((address) >> 16)); /* 发送 bit23 ~ bit16 地址 */
    w25qxx_spi_read_write_byte((uint8_t)((address) >> 8));  /* 发送 bit15 ~ bit8  地址 */
    w25qxx_spi_read_write_byte((uint8_t)address);           /* 发送 bit7  ~ bit0  地址 */
}

/**
 * ******************************************************************************
 * @brief 	: 25QXX 读取状态寄存器，25QXX一共有3个状态寄存器
 * @param 	  regno: 状态寄存器号，范围:1~3
 * @retval 	: 状态寄存器值
 * @author 	: chenningzhan
 * @note	: 状态寄存器1：
 *            BIT7  6   5   4   3   2   1   0
 *            SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 *            SPR:默认0,状态寄存器保护位,配合WP使用
 *            TB,BP2,BP1,BP0:FLASH区域写保护设置
 *            WEL:写使能锁定
 *            BUSY:忙标记位(1,忙;0,空闲)
 *            默认:0x00
 *
 *            状态寄存器2：
 *            BIT7  6   5   4   3   2   1   0
 *            SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
 *
 *            状态寄存器3：
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
        command = FLASH_ReadStatusReg1; /* 读状态寄存器1指令 */
        break;

    case 2:
        command = FLASH_ReadStatusReg2; /* 读状态寄存器2指令 */
        break;

    case 3:
        command = FLASH_ReadStatusReg3; /* 读状态寄存器3指令 */
        break;

    default:
        command = FLASH_ReadStatusReg1;
        break;
    }

    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(command);     /* 发送读寄存器命令 */
    byte = w25qxx_spi_read_write_byte(0Xff); /* 读取一个字节 */
    W25QXX_SPI_UNSELECTED();

    return byte;
}

/**
 * ******************************************************************************
 * @brief 	: 25QXX 写状态寄存器
 * @param 	  regno	: 状态寄存器号，范围:1~3
 * @param 	  sr	: 要写入状态寄存器的值
 * @author 	: chenningzhan
 * @note	: 寄存器说明见w25qxx_read_sr函数说明
 * ******************************************************************************
 */
void w25qxx_write_sr(uint8_t regno, uint8_t sr)
{
    uint8_t command = 0;

    switch (regno)
    {
    case 1:
        command = FLASH_WriteStatusReg1; /* 写状态寄存器1指令 */
        break;

    case 2:
        command = FLASH_WriteStatusReg2; /* 写状态寄存器2指令 */
        break;

    case 3:
        command = FLASH_WriteStatusReg3; /* 写状态寄存器3指令 */
        break;

    default:
        command = FLASH_WriteStatusReg1;
        break;
    }

    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(command); /* 发送读寄存器命令 */
    w25qxx_spi_read_write_byte(sr);      /* 写入一个字节 */
    W25QXX_SPI_UNSELECTED();
}

/**
 * ******************************************************************************
 * @brief 	: 25QXX 读数据
 * @param 	  pbuf  	: 数据存储区
 * @param 	  addr  	: 开始读取的地址(最大32bit)
 * @param 	  datalen  	: 要读取的字节数(最大65535)
 * @author 	: chenningzhan
 * @note	: 在指定地址开始读取指定长度的数据
 * ******************************************************************************
 */
void w25qxx_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i;

    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(FLASH_ReadData); /* 发送读取命令 */
    w25qxx_send_address(addr);                  /* 发送地址 */

    for (i = 0; i < datalen; i++)
    {
        pbuf[i] = w25qxx_spi_read_write_byte(0XFF); /* 循环读取 */
    }

    W25QXX_SPI_UNSELECTED();
}

/**
 * ******************************************************************************
 * @brief 	: 25qxx 在一页(0~65535)内写入少于最大256字节的数据
 * @param 	  pbuf  	: 数据存储区
 * @param 	  addr  	: 开始写入的地址(最大32bit)
 * @param 	  datalen  	: 要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
 * @author 	: chenningzhan
 * @note 	: 最大256字节的数据
 * ******************************************************************************
 */
static void w25qxx_write_page(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i;
    /* 写使能 */
    w25qxx_write_enable();

    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(FLASH_PageProgram);
    w25qxx_send_address(addr);

    for (i = 0; i < datalen; i++)
    {
        w25qxx_spi_read_write_byte(pbuf[i]);
    }

    W25QXX_SPI_UNSELECTED();
    /* 等待写入结束 */
    w25qxx_wait_busy();
}

/**
 * ******************************************************************************
 * @brief 	: 25qxx 无检验写数据
 * @param 	  pbuf  	: 数据存储区
 * @param 	  addr  	: 开始写入的地址(最大32bit)
 * @param 	  datalen  	: 要写入的字节数(最大65535)
 * @author 	: chenningzhan
 * @note 	: 必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
 * ******************************************************************************
 */
static void w25qxx_write_nocheck(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t pageremain;
    pageremain = 256 - addr % 256; /* 单页剩余的字节数 */

    if (datalen <= pageremain) /* 不大于256个字节 */
    {
        pageremain = datalen;
    }

    while (1)
    {
        /* 当写入字节比页内剩余地址还少的时候, 一次性写完
         * 当写入直接比页内剩余地址还多的时候, 先写完整个页内剩余地址, 然后根据剩余长度进行不同处理
         */
        w25qxx_write_page(pbuf, addr, pageremain);

        if (datalen == pageremain) /* 写入结束了 */
        {
            break;
        }
        else /* datalen > pageremain */
        {
            pbuf += pageremain;    /* pbuf指针地址偏移,前面已经写了pageremain字节 */
            addr += pageremain;    /* 写地址偏移,前面已经写了pageremain字节 */
            datalen -= pageremain; /* 写入总长度减去已经写入了的字节数 */

            if (datalen > 256) /* 剩余数据还大于一页,可以一次写一页 */
            {
                pageremain = 256; /* 一次可以写入256个字节 */
            }
            else /* 剩余数据小于一页,可以一次写完 */
            {
                pageremain = datalen; /* 不够256个字节了 */
            }
        }
    }
}

/**
 * ******************************************************************************
 * @brief 	: 25QXX 写数据
 * @param 	  pbuf  	: 数据存储区
 * @param 	  addr  	: 开始写入的地址(最大32bit)
 * @param 	  datalen  	: 要写入的字节数(最大65535)
 * @author 	: chenningzhan
 * @note 	: 在指定地址开始写入指定长度的数据 , 该函数带擦除操作!
 * ******************************************************************************
 */
void w25qxx_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t norflash_buf[4096];

    secpos = addr / 4096;      /* 扇区地址 */
    secoff = addr % 4096;      /* 在扇区内的偏移 */
    secremain = 4096 - secoff; /* 扇区剩余空间大小 */

    if (datalen <= secremain)
    {
        secremain = datalen; /* 不大于4096个字节 */
    }

    while (1)
    {
        w25qxx_read(norflash_buf, secpos * 4096, 4096); /* 读出整个扇区的内容 */

        for (i = 0; i < secremain; i++) /* 校验数据 */
        {
            if (norflash_buf[secoff + i] != 0XFF)
            {
                break; /* 需要擦除, 直接退出for循环 */
            }
        }

        if (i < secremain) /* 需要擦除 */
        {
            w25qxx_erase_sector(secpos); /* 擦除这个扇区 */

            for (i = 0; i < secremain; i++) /* 复制 */
            {
                norflash_buf[i + secoff] = pbuf[i];
            }

            w25qxx_write_nocheck(norflash_buf, secpos * 4096, 4096); /* 写入整个扇区 */
        }
        else /* 写已经擦除了的,直接写入扇区剩余区间. */
        {
            w25qxx_write_nocheck(pbuf, addr, secremain); /* 直接写扇区 */
        }

        if (datalen == secremain)
        {
            break; /* 写入结束了 */
        }
        else /* 写入未结束 */
        {
            secpos++;   /* 扇区地址增1 */
            secoff = 0; /* 偏移位置为0 */

            pbuf += secremain;    /* 指针偏移 */
            addr += secremain;    /* 写地址偏移 */
            datalen -= secremain; /* 字节数递减 */

            if (datalen > 4096)
            {
                secremain = 4096; /* 下一个扇区还是写不完 */
            }
            else
            {
                secremain = datalen; /* 下一个扇区可以写完了 */
            }
        }
    }
}

/**
 * ******************************************************************************
 * @brief 	: 擦除一个扇区
 * @param 	  saddr  	: 扇区地址 根据实际容量设置
 * @author 	: chenningzhan
 * @note 	: 注意,这里是扇区地址,不是字节地址!!
 * 擦除一个扇区的最少时间:150ms
 * ******************************************************************************
 */
void w25qxx_erase_sector(uint32_t saddr)
{
    saddr *= 4096;
    w25qxx_write_enable(); /* 写使能 */
    w25qxx_wait_busy();    /* 等待空闲 */

    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(FLASH_SectorErase); /* 发送写页命令 */
    w25qxx_send_address(saddr);                    /* 发送地址 */
    W25QXX_SPI_UNSELECTED();
    w25qxx_wait_busy(); /* 等待扇区擦除完成 */
}

/**
 * ******************************************************************************
 * @brief 	: 擦除整个芯片
 * @author 	: chenningzhan
 * @note 	:  等待时间超长...
 * ******************************************************************************
 */
void w25qxx_erase_chip(void)
{
    w25qxx_write_enable(); /* 写使能 */
    w25qxx_wait_busy();    /* 等待空闲 */
    W25QXX_SPI_SELECTED();
    w25qxx_spi_read_write_byte(FLASH_ChipErase); /* 发送读寄存器命令 */
    W25QXX_SPI_UNSELECTED();
    w25qxx_wait_busy(); /* 等待芯片擦除结束 */
}