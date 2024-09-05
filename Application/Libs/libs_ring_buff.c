/**
 * ******************************************************************************
 * @Copyright:  Copyright  (C)
 * @file 	: libs_ring_buff.c
 * ******************************************************************************
 * @brief 	: 【introduction】
 * @version	: 1.0
 * @date 	: 2024-08-12
 * @note 	: 【note】
 * *
 * *
 * ******* 1. Improvement *********
 * * 1. improvement the function :
 * *
 * * 2. improvement the function :
 * *
 * *
 * ******* 2. Fix the bug *********
 * * 1. fix the bug of :
 * *
 * *
 * ******************************************************************************
 */
/* ---------------------------- user header file ---------------------------- */

#include "libs_ring_buff.h"

/* ----------------------- public function definition ----------------------- */

rb_status_e ring_buff_clean(ring_buff_t *rb)
{
    if (NULL_PTR == rb->buff)
    {
        return RB_STATUS_ERROR;
    }

    memset(rb->buff, 0x0, rb->buff_size);
    rb->cnt = rb->wi = rb->ri = 0;

    return RB_STATUS_OK;
}

rb_status_e ring_buff_init(ring_buff_t *rb, uint8_t *buf, uint16_t size)
{
    if (buf == NULL_PTR || 0 == size)
        return RB_STATUS_ERROR;

    // set the default parameter
    rb->buff = buf;
    rb->buff_size = size;
    ring_buff_clean(rb);

    return RB_STATUS_OK;
}

uint8_t ring_buff_peek(ring_buff_t *rb)
{
    if (NULL_PTR == rb->buff)
    {
        return RB_STATUS_ERROR;
    }

    return rb->buff[rb->ri];
}

rb_status_e ring_buff_peeks(ring_buff_t *rb, uint8_t *buf, uint16_t size)
{
    if (NULL_PTR == rb->buff)
    {
        return RB_STATUS_ERROR;
    }

    if (size > rb->buff_size)
        return RB_STATUS_OVERFLOW;

    uint16_t old_ri = rb->ri;
    for (int i = 0; i < size; i++)
    {
        buf[i] = rb->buff[rb->ri];
        // 使用模运算实现环绕
        rb->ri = (rb->ri + 1) % rb->buff_size;
    }
    rb->ri = old_ri;

    return RB_STATUS_OK;
}

uint8_t ring_buff_pop(ring_buff_t *rb)
{
    uint8_t read = 0;
    if (rb->cnt)
    {
        rb->cnt--;
        read = rb->buff[rb->ri];
        // 使用模运算实现环绕
        rb->ri = (rb->ri + 1) % rb->buff_size;
    }
    return read;
}

int ring_buff_read(ring_buff_t *rb, uint8_t *buf, uint16_t size)
{
    if (buf == NULL_PTR || 0 == size)
        return RB_STATUS_ERROR;

    int read_size;
    if (size <= rb->cnt)
    {
        read_size = size;
    }
    else
    {
        read_size = 0; // 不满要读的先不读
    }

    for (int i = 0; i < read_size; ++i)
    {
        buf[i] = rb->buff[rb->ri];
        // 使用模运算实现环绕
        rb->ri = (rb->ri + 1) % rb->buff_size;
    }
    rb->cnt -= read_size;

    return read_size;
}

int ring_buff_push(ring_buff_t *rb, uint8_t *buf, uint16_t size)
{
    if (buf == NULL_PTR || 0 == size)
        return RB_STATUS_ERROR;

    int16_t max_write_size;
    int16_t size2end;
    int16_t wi = rb->wi;
    int16_t ri = rb->ri;

    // 做一下 overlap 保护, 单圈情况 wi最大只能=ri-1
    if (wi >= ri)
    {
        size2end = rb->buff_size - 1 - wi;
        max_write_size = size2end + (ri > 0 ? ri - 1 : 0);
    }
    // wi < ri
    else
    {
        max_write_size = ri - 1 - wi;
    }

    if (max_write_size < size)
    {
        return RB_STATUS_OVERFLOW;
    }
    for (int i = 0; i < size; ++i)
    {
        rb->buff[rb->wi] = buf[i];
        // 使用模运算实现环绕
        rb->wi = (rb->wi + 1) % rb->buff_size;
    }

    wi = rb->wi;
    rb->cnt = (wi >= ri) ? (wi - ri) : (wi + rb->buff_size - ri);

    return max_write_size;
}