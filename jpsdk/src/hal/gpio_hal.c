#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "gpiod.h"
#include "log.h"
#include "gpio_hal.h"

typedef struct gpio_hal_handle
{
    gpio_hal_info_t g_hal_info;
    struct gpiod_line_request *wiper_request;
    struct gpiod_line_request *heat_request;
    struct gpiod_line_request *light_request;
}gpio_hal_handle;

gpio_hal_handle g_hal_handle = { 0 };

pthread_mutex_t g_hal_info_mutex = PTHREAD_MUTEX_INITIALIZER;

static int request_chip_gpio(int chip_num, int *offert, enum gpiod_line_direction *dir, bool *active, int num, const char *name, struct gpiod_line_request **requests)
{
    struct gpiod_chip *gpio_chip;
    struct gpiod_line_settings *settings;
    struct gpiod_line_config *line_cfg;
    struct gpiod_request_config *req_cfg;
    char gpio_dev[32];
    int i, ret = 0;

    // 打开GPIO芯片
    snprintf(gpio_dev, sizeof(gpio_dev), "/dev/gpiochip%d", chip_num);
    gpio_chip = gpiod_chip_open(gpio_dev);
    if (!gpio_chip) {
        fprintf(stderr, "open gpio chip %d fail \n", chip_num);
        return -1;
    }

    // 创建设置对象
    settings = gpiod_line_settings_new();
    if (!settings) {
        fprintf(stderr, "setting gpio new fail \n");
        ret = -1;
        goto chip_close;
    }

    // 创建线配置对象
    line_cfg = gpiod_line_config_new();
    if (!line_cfg) {
        fprintf(stderr, "linecfg gpio new fail \n");
        ret = -1;
        goto settings_free;
    }

    // 创建请求配置对象
    req_cfg = gpiod_request_config_new();
    if (!req_cfg) {
        fprintf(stderr, "req_cfg gpio new fail \n");
        ret = -1;
        goto linecfg_free;
    }

    // 为每个GPIO线设置配置
    for (i = 0; i < num; i++) {
        gpiod_line_settings_reset(settings);
        gpiod_line_settings_set_active_low(settings, active[i]);
        gpiod_line_settings_set_direction(settings, dir[i]);

        ret = gpiod_line_config_add_line_settings(line_cfg, (const unsigned int *)(&(offert[i])), 1, settings);
        if (ret) {
            fprintf(stderr, "add_line_settings gpio is fail \n");
            goto reqcfg_free;
        }
    }

    // 设置消费者名称
    gpiod_request_config_set_consumer(req_cfg, name);

    // 请求GPIO线
    *requests = gpiod_chip_request_lines(gpio_chip, req_cfg, line_cfg);
    if (!*requests) {
        fprintf(stderr, "gpiod_chip_request_lines fail \n");
        ret = -1;
    }

reqcfg_free:
    gpiod_request_config_free(req_cfg);
linecfg_free:
    gpiod_line_config_free(line_cfg);
settings_free:
    gpiod_line_settings_free(settings);
chip_close:
    gpiod_chip_close(gpio_chip);
    return ret;
}

int load_gpio_param(gpio_hal_info_t *info)
{
    int ret = 0;
    pthread_mutex_lock(&g_hal_info_mutex);
    memcpy(&(g_hal_handle.g_hal_info), info, sizeof(gpio_hal_info_t));
    g_hal_handle.g_hal_info.valid = 1;
    pthread_mutex_unlock(&g_hal_info_mutex);
    enum gpiod_line_direction dir = g_hal_handle.g_hal_info.wiper.dir == 1 ? GPIOD_LINE_DIRECTION_OUTPUT : GPIOD_LINE_DIRECTION_INPUT;
    bool active = g_hal_handle.g_hal_info.wiper.active_low  != 1 ? false : true; 
    ret = request_chip_gpio(g_hal_handle.g_hal_info.wiper.chip, &(g_hal_handle.g_hal_info.wiper.line), &dir, &active, 1, "wiper_pin", &(g_hal_handle.wiper_request));
    if(ret != 0)
    {
        fprintf(stderr, "load_gpio_param  wiper_pin fail \n");
        return -1;
    }
    dir = g_hal_handle.g_hal_info.heat.dir == 1 ? GPIOD_LINE_DIRECTION_OUTPUT : GPIOD_LINE_DIRECTION_INPUT;
    active = g_hal_handle.g_hal_info.heat.active_low  != 1 ? false : true; 
    ret = request_chip_gpio(g_hal_handle.g_hal_info.heat.chip, &(g_hal_handle.g_hal_info.heat.line), &dir, &active, 1, "heat_pin", &(g_hal_handle.heat_request));
    if(ret != 0)
    {
        fprintf(stderr, "load_gpio_param  heat fail \n");
        return -1;
    }
    dir = g_hal_handle.g_hal_info.light.dir == 1 ? GPIOD_LINE_DIRECTION_OUTPUT : GPIOD_LINE_DIRECTION_INPUT;
    active = g_hal_handle.g_hal_info.light.active_low  != 1 ? false : true; 
    ret = request_chip_gpio(g_hal_handle.g_hal_info.light.chip, &(g_hal_handle.g_hal_info.light.line), &dir, &active, 1, "light_pin", &(g_hal_handle.light_request));
    if(ret != 0)
    {
        fprintf(stderr, "load_gpio_param  light fail \n");
        return -1;
    }
    return 0;
}

int wiper_pin_status_get()
{
    int ret = 0;
    enum gpiod_line_value value;
    value = gpiod_line_request_get_value(g_hal_handle.wiper_request, g_hal_handle.g_hal_info.wiper.line);
    if (value == GPIOD_LINE_VALUE_ACTIVE)
		ret = 1;
	else if (value == GPIOD_LINE_VALUE_INACTIVE) {
		ret = 0;
	} else {
		fprintf(stderr, "error reading value: %s\n",
			strerror(errno));
		ret = -1;
	}
    return ret;
}

int wiper_pin_status_set(int value)
{
    int ret = 0;
    enum gpiod_line_value values;
    values = value == 1 ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;
    ret = gpiod_line_request_set_values_subset(g_hal_handle.wiper_request, 1, (const unsigned int *)(&(g_hal_handle.g_hal_info.wiper.line)), &values);
    if(ret != 0)
    {
        fprintf(stderr, "set reset pin gpio is err \n");
    }
    return ret;
}

int heat_pin_status_set(int value)
{
    int ret = 0;
    enum gpiod_line_value values;
    values = value == 1 ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;
    ret = gpiod_line_request_set_values_subset(g_hal_handle.heat_request, 1, (const unsigned int *)(&(g_hal_handle.g_hal_info.heat.line)), &values);
    if(ret != 0)
    {
        fprintf(stderr, "set reset pin gpio is err \n");
    }
    return ret;
}

int light_pin_status_set(int value)
{
    int ret = 0;
    enum gpiod_line_value values;
    values = value == 1 ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;
    ret = gpiod_line_request_set_values_subset(g_hal_handle.light_request, 1, (const unsigned int *)(&(g_hal_handle.g_hal_info.light.line)), &values);
    if(ret != 0)
    {
        fprintf(stderr, "set reset pin gpio is err \n");
    }
    return ret;
}