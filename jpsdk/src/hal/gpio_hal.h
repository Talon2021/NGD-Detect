
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
    struct gpio_info wiper;
    struct gpio_info heat;
    struct gpio_info light;
} gpio_hal_info_t;


int load_gpio_param(gpio_hal_info_t *info);

int wiper_pin_status_set(int value);

int heat_pin_status_set(int value);

int light_pin_status_set(int value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif