/* ---------------------------- user header file ---------------------------- */

#include "task_led.h"
#include "device_led.h"

/* --------------------------- function definition -------------------------- */

/**
 * ******************************************************************************
 * @brief 	: Entry function for the led handling task.
 * @param 	  p_this  	: a pointer to get the task object
 * @author 	: chenningzhan
 * @note	: enters a loop to Blink a LED
 * ******************************************************************************
 */
void mytask_led_entry(void *p_this)
{
    schedule_t *task = (schedule_t *)p_this;
    static led_device_t *p_dev_r = NULL_PTR;
    static led_device_t *p_dev_g = NULL_PTR;

    /* the task initialization */
    if (0 == task->params.count)
    {
        p_dev_r = led_device_get_pointer(LED_ID_R);
        p_dev_g = led_device_get_pointer(LED_ID_G);
    }
    /* the task starts a loop */
    else
    {
        p_dev_g->ops.control((void *)p_dev_g, LED_STATUS_BLINKING);
    }
}
