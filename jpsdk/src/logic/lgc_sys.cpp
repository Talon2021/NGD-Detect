#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "lgc_sys.h"
#include "meida_common.h"
#include "gpio_hal.h"
#include "mpp_venc.h"

#define TEMPERATURE_AD_SENSOR       "/dev/ves"


SystemInfo_t g_SystemInfo = { 0 };


static void* check_brightness_thread(void* arg)
{
    SystemInfo_t* pSysInfo = (SystemInfo_t*)arg;
    while (1)
    {
        if (0 != pSysInfo->m_bExitCheckBrightness)
        {
            break;
        }
        pthread_mutex_lock(&pSysInfo->m_CheckBrightnessLock);
        if(pSysInfo->m_AutoLightMode == 2)  //自动检测开启
        {

        }
        pthread_mutex_unlock(&pSysInfo->m_CheckBrightnessLock);
        usleep(100 *1000);
    }

    return NULL;
}

typedef struct {
    int temp;      // 温度（摄氏度）
    float R_Min;   // R 最小值 (Kohm)
    float R_Cent;  // R 中心值 (Kohm)
    float R_Max;   // R 最大值 (Kohm)
} TempResistance;
static TempResistance table[] = {
    {-40, 189.674, 197.390, 205.400},
    {-39, 179.349, 186.540, 194.000},
    {-38, 169.647, 176.350, 183.299},
    {-37, 160.550, 166.800, 173.276},
    {-36, 151.990, 157.820, 163.857},
    {-35, 143.951, 149.390, 155.019},
    {-34, 136.431, 141.510, 146.763},
    {-33, 129.347, 134.090, 138.993},
    {-32, 122.680, 127.110, 131.687},
    {-31, 116.391, 120.530, 124.804},
    {-30, 110.472, 114.340, 118.332}, 
    {-29, 104.913, 108.530, 112.260},
    {-28, 99.658, 103.040, 106.526},
    {-27, 94.706, 97.870, 101.129}, 
    {-26, 90.029, 92.989, 96.037}, 
    {-25, 85.611, 88.381, 91.231}, 
    {-24, 81.443, 84.036, 86.702}, 
    {-23, 77.504, 79.931, 82.426}, 
    {-22, 73.779, 76.052, 78.387}, 
    {-21, 70.256, 72.384, 74.569},
    {-20, 66.922, 68.915, 70.961},
    {-19, 63.767, 65.634, 67.549}, 
    {-18, 60.779, 62.529, 64.323}, 
    {-17, 57.949, 59.589, 61.269}, 
    {-16, 55.268, 56.804, 58.377},
    {-15, 52.726, 54.166, 55.640}, 
    {-14, 50.315, 51.665, 53.046},
    {-13, 48.029, 49.294, 50.588}, 
    {-12, 45.860, 47.046, 48.258}, 
    {-11, 43.801, 44.913, 46.049}, 
    {-10, 41.846, 42.889, 43.953}, 
    {-9, 39.989, 40.967, 41.964},
    {-8, 38.225, 39.142, 40.077}, 
    {-7, 36.549, 37.408, 38.284}, 
    {-6, 34.955, 35.761, 36.582}, 
    {-5, 33.440, 34.196, 34.965}, 
    {-4, 31.998, 32.707, 33.428}, 
    {-3, 30.627, 31.291, 31.966}, 
    {-2, 29.322, 29.945, 30.578}, 
    {-1, 28.080, 28.664, 29.257}, 
    {0, 26.898, 27.445, 28.001},
    {1, 25.770, 26.283, 26.804},
    {2, 24.696, 25.177, 25.665}, 
    {3, 23.673, 24.124, 24.581}, 
    {4, 22.699, 23.121, 23.549}, 
    {5, 21.769, 22.165, 22.566}, 
    {6, 20.882, 21.253, 21.628}, 
    {7, 20.037, 20.384, 20.735}, 
    {8, 19.230, 19.555, 19.883}, 
    {9, 18.460, 18.764, 19.071}, 
    {10, 17.725, 18.010, 18.297}, 
    {11, 17.024, 17.290, 17.559}, 
    {12, 16.353, 16.602, 16.853}, 
    {13, 15.713, 15.946, 16.181}, 
    {14, 15.101, 15.319, 15.538}, 
    {15, 14.517, 14.720, 14.925}, 
    {16, 13.958, 14.148, 14.339}, 
    {17, 13.424, 13.601, 13.779}, 
    {18, 12.913, 13.078, 13.244}, 
    {19, 12.424, 12.578, 12.733}, 
    {20, 11.955, 12.099, 12.243},
    {21, 11.508, 11.642, 11.776},
    {22, 11.079, 11.204, 11.329},
    {23, 10.669, 10.785, 10.901}, 
    {24, 10.276, 10.384, 10.492}, 
    {25, 9.900, 10.000, 10.100}, 
    {26, 9.532, 9.632, 9.732}, 
    {27, 9.180, 9.280, 9.380}, 
    {28, 8.843, 8.943, 9.042}, 
    {29, 8.520, 8.619, 8.718}, 
    {30, 8.211, 8.309, 8.408}, 
    {31, 7.914, 8.012, 8.110}, 
    {32, 7.630, 7.727, 7.824}, 
    {33, 7.357, 7.453, 7.550}, 
    {34, 7.096, 7.191, 7.287}, 
    {35, 6.845, 6.939, 7.034}, 
    {36, 6.604, 6.698, 6.792}, 
    {37, 6.373, 6.466, 6.559}, 
    {38, 6.152, 6.243, 6.335}, 
    {39, 5.939, 6.029, 6.120}, 
    {40, 5.735, 5.824, 5.914}, 
    {41, 5.538, 5.627, 5.716}, 
    {42, 5.350, 5.437, 5.525},
    {44, 4.995, 5.080, 5.165}, 
    {45, 4.828, 4.911, 4.996}, 
    {46, 4.667, 4.749, 4.832}, 
    {47, 4.512, 4.593, 4.675},
    {48, 4.363, 4.443, 4.524}, 
    {49, 4.220, 4.299, 4.379}, 
    {50, 4.083, 4.160, 4.239}, 
    {51, 3.950, 4.027, 4.104}, 
    {52, 3.823, 3.898, 3.974}, 
    {53, 3.700, 3.774, 3.849}, 
    {54, 3.582, 3.654, 3.728}, 
    {55, 3.468, 3.539, 3.612}, 
    {56, 3.358, 3.429, 3.500}, 
    {57, 3.252, 3.322, 3.392}, 
    {58, 3.151, 3.219, 3.288}, 
    {59, 3.052, 3.119, 3.188}, 
    {60, 2.958, 3.024, 3.091},
    {61, 2.867, 2.931, 2.997}, 
    {62, 2.779, 2.842, 2.907}, 
    {63, 2.694, 2.756, 2.820},
    {64, 2.612, 2.673, 2.736}, 
    {65, 2.533, 2.593, 2.655},
    {66, 2.457, 2.516, 2.576}, 
    {67, 2.383, 2.441, 2.501}, 
    {68, 2.312, 2.369, 2.428}, 
    {69, 2.244, 2.300, 2.357}, 
    {70, 2.177, 2.233, 2.289},
    {71, 2.113, 2.168, 2.223}, 
    {72, 2.052, 2.105, 2.159},
    {73, 1.992, 2.044, 2.098}, 
    {74, 1.934, 1.986, 2.038}, 
    {75, 1.879, 1.929, 1.981}, 
    {76, 1.825, 1.874, 1.925}, 
    {77, 1.773, 1.821, 1.871}, 
    {78, 1.722, 1.770, 1.819}, 
    {79, 1.673, 1.720, 1.768}, 
    {80, 1.626, 1.673, 1.720}, 
    {81, 1.581, 1.626, 1.672}, 
    {82, 1.537, 1.581, 1.627}, 
    {83, 1.494, 1.538, 1.582}, 
    {84, 1.453, 1.496, 1.540}, 
    {85, 1.413, 1.455, 1.498},
    {86, 1.374, 1.416, 1.458}, 
    {87, 1.337, 1.377, 1.419}, 
    {88, 1.300, 1.340, 1.381}, 
    {89, 1.265, 1.304, 1.345}, 
    {90, 1.231, 1.270, 1.309},
    {91, 1.198, 1.236, 1.275}, 
    {92, 1.167, 1.204, 1.242}, 
    {93, 1.136, 1.172, 1.209}, 
    {94, 1.106, 1.141, 1.178}, 
    {95, 1.076, 1.112, 1.148}, 
    {96, 1.048, 1.083, 1.118}, 
    {97, 1.021, 1.055, 1.090}, 
    {98, 0.995, 1.028, 1.062}, 
    {99, 0.969, 1.002, 1.035}, 
    {100, 0.944, 0.976, 1.009} 
};


static int findTemperatureByResistance(float R) {
    int n = sizeof(table) / sizeof(table[0]);
    float temp = 0;
    if(R > 197.390)
    {
        temp = -40;
    }
    if(R < 0.976)
    {
        temp = 100;
    }
    for (int i = 0; i < n - 1; i++) {
        
        if (R <= table[i].R_Cent && R >= table[i + 1].R_Cent) {
            
            float R1 = table[i].R_Cent;
            float R2 = table[i + 1].R_Cent;
            float T1 = table[i].temp;
            float T2 = table[i + 1].temp;
            
           
            temp = T1 + ((R - R1) / (R2 - R1)) * (T2 - T1);
            return (int)temp;  
        }
    }
   
    return (int)temp;
}

static int deal_temctrl(int ch, int tem, TemperatureCfg cfg)
{
    SystemInfo_t* pSysInfo = NULL;
    LGC_SYS_GetSystemInfo(&pSysInfo);
    int mid_tem =  cfg.min_temperature + ((cfg.max_temperature - cfg.min_temperature) / 3);

    if(tem > 50)
    {
        SDK_DBG("Exceeding rated temperature \n");
        set_bit(&pSysInfo->m_TemCtrlFlag, ch-1, 0);     
        heat_pin_status_set(ch, 0);
        SelectSleepMSec(50); 

        return 0;  
    }
#if 1
    if(abs(tem - mid_tem) < 1)
    {
        if(get_bit(&pSysInfo->m_TemCtrlFlag, ch-1))      //如果打开，就关闭
        {
            set_bit(&pSysInfo->m_TemCtrlFlag, ch-1, 0);     
            heat_pin_status_set(ch, 0);
            SelectSleepMSec(50);
        }
    }
    else
    {
        if(tem < cfg.min_temperature)           
        {
            if(!get_bit(&pSysInfo->m_TemCtrlFlag, ch-1))     //如果关闭就打开
            {
                set_bit(&pSysInfo->m_TemCtrlFlag, ch-1, 1);     
                heat_pin_status_set(ch, 1);
                SelectSleepMSec(50);
            }
           
        }
        else if (tem > cfg.max_temperature)
        {
            if(get_bit(&pSysInfo->m_TemCtrlFlag, ch-1))      //如果打开，就关闭
            {
                set_bit(&pSysInfo->m_TemCtrlFlag, ch-1, 0);     
                heat_pin_status_set(ch, 0);
                SelectSleepMSec(50);
            }
        }
    }
#endif
    return 0;
}

#define CALC_RX(A, R5) (((A) * (R5)) / (1.8f - (A)))
static void* check_temperature_thread(void* arg)
{
    SystemInfo_t* pSysInfo = (SystemInfo_t*)arg;
    FILE *fd2 = fopen("/sys/bus/iio/devices/iio:device0/in_voltage3_raw", "r");        //加热圈2
    FILE *fd1 = fopen("/sys/bus/iio/devices/iio:device0/in_voltage4_raw", "r");        //加热圈1
    if(fd2 == 0 || fd1 == 0)
    {
        SDK_ERR("open file is fail \n");
        return NULL;
    }
    int ret = 0;
    char buff[32] = {0};
    float Voltage;
    float Rx;
    int temp1 = 0, temp2 = 0; 

    SDK_DBG("start temperateure check");
    while (1)
    {
        if (0 != pSysInfo->m_bExitTemperature)
        {
            break;
        }
        pthread_mutex_lock(&pSysInfo->m_CheckTemperatureLock);
        if(pSysInfo->m_AutoTemperatureMode == 2)  //自动检测开启
        {
            memset(buff, 0, sizeof(buff));
            ret = fread(buff, 1, 32, fd1);
            if(ret > 0)
            {
                Voltage = (atof(buff)) * 1.8f / 4096.0f;
                Rx = CALC_RX(Voltage, 1);
                temp1 = findTemperatureByResistance(Rx);
                deal_temctrl(1, temp1, pSysInfo->m_autoTemCtrlCfg);
                
            }
            memset(buff, 0, sizeof(buff));
            ret = fread(buff, 1, 32, fd2);
            if(ret > 0)
            {
                Voltage = (atof(buff)) * 1.8f / 4096.0f;
                Rx = CALC_RX(Voltage, 1);
                temp2 = findTemperatureByResistance(Rx);
                deal_temctrl(2, temp2, pSysInfo->m_autoTemCtrlCfg);
            }
            fseek(fd1, 0, SEEK_SET);
            fseek(fd2, 0, SEEK_SET);
            SDK_DBG("temp1 = %d temp2 = %d \n", temp1, temp2);

        }
        pthread_mutex_unlock(&pSysInfo->m_CheckTemperatureLock);
        sleep(1);
    }

    fclose(fd2);
    fclose(fd1);
    return NULL;
}


void StartCheckBrightness()
{
    SystemInfo_t* pSysInfo = NULL;
    LGC_SYS_GetSystemInfo(&pSysInfo);
    pSysInfo->m_bExitCheckBrightness = 0;
    pthread_create(&pSysInfo->m_CheckBrightnessThread, NULL, check_brightness_thread, (void*)pSysInfo);
    return;
}


void StopCheckBrightness()
{
    SystemInfo_t* pSysInfo = NULL;
    LGC_SYS_GetSystemInfo(&pSysInfo);
    pSysInfo->m_bExitCheckBrightness = 1;
    pthread_join(pSysInfo->m_CheckBrightnessThread, NULL);
    return;
}

void StartCheckTemperature()
{
    SystemInfo_t* pSysInfo = NULL;
    LGC_SYS_GetSystemInfo(&pSysInfo);
    pSysInfo->m_bExitTemperature = 0;
    pthread_create(&pSysInfo->m_CheckTemperatureThread, NULL, check_temperature_thread, (void*)pSysInfo);
    return;
}

void StopCheckTemperature()
{
    SystemInfo_t* pSysInfo = NULL;
    LGC_SYS_GetSystemInfo(&pSysInfo);
    pSysInfo->m_bExitTemperature = 1;
    pthread_join(pSysInfo->m_CheckTemperatureThread, NULL);
    return;
}

int LGC_SYS_Init()
{
    SystemInfo_t* pSysInfo = &g_SystemInfo;
    memset(pSysInfo, 0, sizeof(SystemInfo_t));
    pthread_mutex_init(&pSysInfo->m_CheckBrightnessLock, NULL);
    pthread_mutex_init(&pSysInfo->m_CheckTemperatureLock, NULL);
    pSysInfo->m_AutoTemperatureMode = 2;
    if(mpp_venc_init())
    {
        SDK_ERR("Fail to called mpp_venc_init\n");
    }

    heat_pin_status_set(1, 0);
    SelectSleepMSec(50);      
    heat_pin_status_set(2, 0);
    SelectSleepMSec(50);
    pSysInfo->m_TemCtrlFlag = 0;
    pSysInfo->m_autoTemCtrlCfg.min_temperature = 0;
    pSysInfo->m_autoTemCtrlCfg.max_temperature = 0;
    StartCheckTemperature();


    return 0;
}

void LGC_SYS_DeInit()
{
    SystemInfo_t* pSysInfo = &g_SystemInfo;
    if(mpp_venc_deinit())
    pthread_mutex_destroy(&pSysInfo->m_CheckBrightnessLock);
    pthread_mutex_destroy(&pSysInfo->m_CheckTemperatureLock);
}

int LGC_SYS_GetSystemInfo(SystemInfo_t **pInfo)
{
    *pInfo = &g_SystemInfo;
    return 0;
}

int LGC_SYS_SetLightMode(int mode)
{
    SystemInfo_t* pSysInfo = NULL;
    if (HY_Res_SDK_Lock() != 0)
    {
        SDK_ERR("SDK Not Init\n");
        return -1;
    }
    LGC_SYS_GetSystemInfo(&pSysInfo);

    pthread_mutex_lock(&pSysInfo->m_CheckBrightnessLock);
    pSysInfo->m_AutoLightMode = mode;
    pthread_mutex_unlock(&pSysInfo->m_CheckBrightnessLock);

    HY_Res_SDK_UnLock();
    return 0;
}

int LGC_SYS_SetheatMode(int mode)
{
    SystemInfo_t* pSysInfo = NULL;
    if (HY_Res_SDK_Lock() != 0)
    {
        SDK_ERR("SDK Not Init\n");
        return -1;
    }
    LGC_SYS_GetSystemInfo(&pSysInfo);

    pthread_mutex_lock(&pSysInfo->m_CheckTemperatureLock);
    heat_pin_status_set(1, 0);      //切换模式先关闭加热圈
    SelectSleepMSec(50);
    heat_pin_status_set(2, 0);
    pSysInfo->m_TemCtrlFlag = 0;
    pSysInfo->m_AutoTemperatureMode = mode;
    

    pthread_mutex_unlock(&pSysInfo->m_CheckTemperatureLock);


    HY_Res_SDK_UnLock();
    return 0;
}

int LGC_SYS_SetLightEnable(int enable)
{
    SystemInfo_t* pSysInfo = NULL;
    if (HY_Res_SDK_Lock() != 0)
    {
        SDK_ERR("SDK Not Init\n");
        return -1;
    }
    LGC_SYS_GetSystemInfo(&pSysInfo);

    pthread_mutex_lock(&pSysInfo->m_CheckBrightnessLock);

    pthread_mutex_unlock(&pSysInfo->m_CheckBrightnessLock);


    HY_Res_SDK_UnLock();
    return 0;
}

int LGC_SYS_SetheatEnable(int ch, int enable)
{
    int ret = 0;
    SystemInfo_t* pSysInfo = NULL;
    if (HY_Res_SDK_Lock() != 0)
    {
        SDK_ERR("SDK Not Init\n");
        return -1;
    }
    LGC_SYS_GetSystemInfo(&pSysInfo);

    pthread_mutex_lock(&pSysInfo->m_CheckTemperatureLock);
    set_bit(&pSysInfo->m_TemCtrlFlag, ch-1, enable);
    ret = heat_pin_status_set(ch, enable);
    SDK_DBG("heat ch = %d ret = %d enable\n", ch, ret, enable);
    pthread_mutex_unlock(&pSysInfo->m_CheckTemperatureLock);


    HY_Res_SDK_UnLock();
    return 0;
}

int LGC_SYS_SetTemCtrlCfg(TemperatureCfg cfg)
{
     SystemInfo_t* pSysInfo = NULL;
    if (HY_Res_SDK_Lock() != 0)
    {
        SDK_ERR("SDK Not Init\n");
        return -1;
    }
    LGC_SYS_GetSystemInfo(&pSysInfo);
    pthread_mutex_lock(&pSysInfo->m_CheckTemperatureLock);
    memcpy(&pSysInfo->m_autoTemCtrlCfg, &cfg, sizeof(TemperatureCfg));
    pthread_mutex_unlock(&pSysInfo->m_CheckTemperatureLock);
    HY_Res_SDK_UnLock();
    return 0;
}
