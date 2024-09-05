#ifndef __LIBS_RING_BUFF_H__
#define __LIBS_RING_BUFF_H__

/* ---------------------------- user header file ---------------------------- */
#include "./user_libs.h"

/* ---------------------------- macro definition ---------------------------- */

/**
 * @brief NULL definition safeguard.
 */
#ifndef NULL_PTR
#define NULL_PTR (void *)(0)
#endif

/* --------------------------- struct definitions --------------------------- */

typedef enum
{
    RB_STATUS_ERROR = -1, // Buffer error status
    RB_STATUS_OK = 0,     // Buffer Success status
    RB_STATUS_OVERFLOW,   // Buffer overflow
} rb_status_e;

typedef struct rb_t
{
    uint8_t *buff;
    vuint16_t buff_size;
    vuint16_t cnt;
    vuint16_t wi;
    vuint16_t ri;
} ring_buff_t;

/* -------------------------- function declaration -------------------------- */
rb_status_e ring_buff_init(ring_buff_t *rb, uint8_t *buf, uint16_t size);

rb_status_e ring_buff_clean(ring_buff_t *rb);

uint8_t ring_buff_peek(ring_buff_t *rb);

rb_status_e ring_buff_peeks(ring_buff_t *rb, uint8_t *buf, uint16_t size);

uint8_t ring_buff_pop(ring_buff_t *rb);

int ring_buff_read(ring_buff_t *rb, uint8_t *buf, uint16_t size);

int ring_buff_push(ring_buff_t *rb, uint8_t *buf, uint16_t size);

#endif
