#include <stdio.h>
#include "lgc.h"
#include "gpio_hal.h"

static void load_board_hal_info()
{
    gpio_hal_info_t info = {0};
    info.valid = 1;

    info.heat_1.chip = 2;
    info.heat_1.line = 18;       //GPIO2 C2
    info.heat_1.active_low = 0;
    info.heat_1.dir = 1;

    info.heat_2.chip = 2;
    info.heat_2.line = 8;       //GPIO2 B0 
    info.heat_2.active_low = 0;
    info.heat_2.dir = 1;

    info.mcu_power.chip = 2;
    info.mcu_power.line = 6;       //GPIO2 A6
    info.mcu_power.active_low = 0;
    info.mcu_power.dir = 1;

    info.ir_power.chip = 1;
    info.ir_power.line = 3;       //GPIO1 A3
    info.ir_power.active_low = 0;
    info.ir_power.dir = 1;

    info.vis_power.chip = 1;
    info.vis_power.line = 2;       //GPIO1 A2
    info.vis_power.active_low = 0;
    info.vis_power.dir = 1;

    info.ptz_uart.chip = 4;
    info.ptz_uart.line = 9;       //GPIO4 B1 
    info.ptz_uart.active_low = 0;
    info.ptz_uart.dir = 1;

    info.alarm_uart.chip = 4;
    info.alarm_uart.line = 17;       //GPI04 C1
    info.alarm_uart.active_low = 0;
    info.alarm_uart.dir = 1;


    int ret = load_gpio_param(&info);
    if (ret != 0)
    {
        fprintf(stderr, "Board Hal Info Load Faile! ret[%d]", ret);
    }
    fprintf(stderr, "load gpio is success \n");
    return;
}

int LGC_Init()
{
#ifdef LOAD_GPIO
    load_board_hal_info();
#endif

    LGC_SYS_Init();
    LGC_ALGO_Init();

    return 0;
}

void LGC_DeInit()
{
    LGC_ALGO_DeInit();
    LGC_SYS_DeInit();
}
