/* ---------------------------- user header file ---------------------------- */

#include "./device.h"
#include "./device_buzzer.h"
#include "./device_can.h"
#include "./device_led.h"

/* ----------------------- global variable definition ----------------------- */

p_func_register_dev reg_list[] = {
    led_device_register,
    buzzer_device_register,
};

/* ----------------------- public function definition ----------------------- */

void device_register(void)
{
    for (uint8_t i = 0; i < ARRAY_NUM(reg_list); i++)
    {
        reg_list[i](NULL_PTR);
    }
}
