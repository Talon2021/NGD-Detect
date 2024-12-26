#include <stdio.h>
#include "lgc.h"
#include "gpio_hal.h"

static void load_board_hal_info()
{
    gpio_hal_info_t info;
    info.valid = 1;

    info.wiper.chip = 0;
    info.wiper.line = 17;
    info.wiper.active_low = 0;
    info.wiper.dir = 1;

    info.heat.chip = 0;
    info.heat.line = 18;
    info.heat.active_low = 0;
    info.heat.dir = 1;

    info.light.chip = 0;
    info.light.line = 19;
    info.light.active_low = 0;
    info.light.dir = 1;
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
