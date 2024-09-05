#ifndef __LIBS_CRC_H__
#define __LIBS_CRC_H__

/* ---------------------------- user header file ---------------------------- */
#include "./user_libs.h"

/* ---------------------------- macro definition ---------------------------- */

#define CRC8_INIT (0x0000u)
#define CRC16_INIT (0x0000u)
#define CRC32_INIT (0x0000u)

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef __FALSE
#define __FALSE (0)
#endif

#ifndef __TRUE
#define __TRUE (1)
#endif

/* -------------------------- function declaration -------------------------- */

uint8_t crc8_calc(uint8_t *pchMessage, uint32_t dwLength, uint8_t ucCRC8);
uint32_t crc8_verify(uint8_t *pchMessage, uint32_t dwLength);
void crc8_append(uint8_t *pchMessage, uint32_t dwLength);

uint16_t crc16_calc(uint8_t *pchMessage, uint32_t dwLength, uint16_t wCRC);
uint32_t crc16_verify(uint8_t *pchMessage, uint32_t dwLength);
void crc16_append(uint8_t *pchMessage, uint32_t dwLength);

uint32_t crc32_calc(uint8_t *pchMessage, uint32_t dwLength, uint32_t wCRC);
uint32_t crc32_verify(uint8_t *pchMessage, uint32_t dwLength);
void crc32_append(uint8_t *pchMessage, uint32_t dwLength);

#endif
