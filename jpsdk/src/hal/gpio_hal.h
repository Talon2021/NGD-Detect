
#ifndef _GPIO_HAL_H
#define _GPIO_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

struct gpio_info {
    int chip; //number of the gpiochip
    int line; // Offset of the GPIO line.
    int active_low; // The active state of this line - true if low.
    int dir;        //0 intput 1 ouput
};


typedef struct gpio_hal_info {
    int valid;
    struct gpio_info heat_1;        //加热圈1
    struct gpio_info heat_2;        //加热圈2
    struct gpio_info mcu_power;      //mcu 供电
    struct gpio_info ir_power;      //红外机芯供电
    struct gpio_info vis_power;     //可见光机芯供电
    struct gpio_info ptz_uart;      //云台切换
    struct gpio_info alarm_uart;    //预警控制
} gpio_hal_info_t;


int load_gpio_param(gpio_hal_info_t *info);

int heat_pin_status_set(int ch, int value);

int mcu_pin_power_set(int value);

int ir_pin_power_set(int value);

int vis_pin_power_set(int value);

int ptz_pin_power_set(int value);

int alarm_pin_power_set(int value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif