#include "gpio.h"
#include <stdio.h>
//#include "gpioInst.h"
#include "memmap.h"
// #include "/home/lfz/Hi3531DV200_SDK_V2.0.0.3/osdrv/tools/board/reg-tools-1.0.0/include/utils/memmap.h"

// 注意�?3536c没有gpio4
//  #define GPIO_0_BASE 0x102f0000
//  #define GPIO_1_BASE 0x10ff0058
//  #define GPIO_2_BASE 0x102f0020
//  #define GPIO_3_BASE 0x102f0040
//  #define GPIO_4_BASE 0x102f0060
//  #define GPIO_5_BASE 0x102f0080
//  #define GPIO_6_BASE 0x102f00A0          // GPIO6_1
//  #define GPIO_7_BASE 0x17C70018
//  #define GPIO_8_BASE 0x17C70038
//  #define GPIO_9_BASE 0x17C70058
//  #define GPIO_10_BASE 0x17C70078
//  #define GPIO_11_BASE 0x17C70098
//  #define GPIO_12_BASE 0x17C700B8
//  #define GPIO_13_BASE 0x17C700D8
//  #define GPIO_14_BASE 0x17C700F8
//  #define GPIO_15_BASE 0x17C70118
//  #define GPIO_16_BASE 0x17C70138
//  #define GPIO_17_BASE 0x17C70158
//  #define GPIO_18_BASE 0x17C70178
//  #define GPIO_19_BASE 0x17C70198
//  #define GPIO_20_BASE 0x17C701B8
//  #define GPIO_21_BASE 0x17C701D8
//  #define GPIO_22_BASE 0x17C701F8

#define GPIO_0_BASE 0x11090000
#define GPIO_1_BASE 0x11091000
#define GPIO_2_BASE 0x11092000
#define GPIO_3_BASE 0x11093000
#define GPIO_4_BASE 0x11094000
#define GPIO_5_BASE 0x11095000
#define GPIO_6_BASE 0x11096000
#define GPIO_7_BASE 0x11097000
#define GPIO_8_BASE 0x11098000
#define GPIO_9_BASE 0x11099000
#define GPIO_10_BASE 0x1109a000
#define GPIO_11_BASE 0x1109b000
#define GPIO_12_BASE 0x1109c000
#define GPIO_13_BASE 0x1109d000
#define GPIO_14_BASE 0x1109e000
#define GPIO_15_BASE 0x1109f000
#define GPIO_16_BASE 0x110a0000
#define GPIO_17_BASE 0x110a1000
#define GPIO_18_BASE 0x110a2000
#define GPIO_19_BASE 0x110a3000
#define GPIO_20_BASE 0x110a4000
#define GPIO_21_BASE 0x110a5000
#define GPIO_22_BASE 0x110a6000
#define GPIO_23_BASE 0x110a7000
#define GPIO_24_BASE 0x110a8000
#define GPIO_25_BASE 0x110a9000

static unsigned int GPIO_BASE[3] =
    {
        0xFF460000,
        0xFF620000,
        0xFF630000};

// unsigned int GetGpioBase(unsigned int group)
// {
// 	switch (group)
// 	{
// 	   case  0: return 0x12150000; break;
//     case  1: return 0x12160000; break;
//     case  2: return 0x12170000; break;
//     case  3: return 0x12180000; break;
//     case  4: return 0x12190000; break;
//     case  5: return 0x121A0000; break;
//     case  6: return 0x121B0000; break;
//     case  7: return 0x121C0000; break;
//     case  8: return 0x121D0000; break;
//     case  9: return 0x121E0000; break;
//     case 10: return 0x121F0000; break;
//     case 11: return 0x12200000; break;
//     case 12: return 0x12210000; break;
//     case 13: return 0x12220000; break;
//     case 14: return 0x12230000; break;
//     case 15: return 0x12240000; break;
//     case 16: return 0x12250000; break;
//     case 17: return 0x12260000; break;
//     case 18: return 0x12270000; break;
//     case 19: return 0x12280000; break;
//     case 20: return 0x12290000; break;
//     case 21: return 0x122A0000; break;
//     case 22: return 0x122B0000; break;
//     case 23: return 0x122C0000; break;
//     case 24: return 0x122D0000; break;
// 	default:
// 		break;
// 	}
// 	return 0;
// }

void SetGpioDir(unsigned int group, unsigned int pin, unsigned int dir)
{
    unsigned int addr = GPIO_BASE[group];
    unsigned int offset = 0x0008 + (pin / 16) * 4;
    addr |= offset;
    unsigned int iDir = dir;
    unsigned int value = getHimm(addr);
    value &= ~(0x01 << pin%16);
    value |= (0x01 << (16 + pin%16));
    value |= (iDir << pin%16);
    himm(addr, value);
}

unsigned int GetGpioDir(unsigned int group, unsigned int pin)
{
    unsigned int addr = GPIO_BASE[group];
    unsigned int offset = 0x0008 + (pin / 16) * 4;
    addr |= offset;
    // unsigned int iDir = dir;
    // unsigned int value = getHimm(addr);
    // value &= ~(0x01 << pin);
    // value |= (0x01 << (16 + pin));
    // value |= (iDir << pin);
    // himm(addr, value);

    // unsigned int setPin = (0x04 << pin);
    unsigned int value = getHimm(addr);
    value = value >> (pin % 16);
    return value & 0x01;
}

void SetGpioGroupDirs(unsigned int group, unsigned int pins, unsigned int dirs)
{
    unsigned int dir = getHimm(GPIO_BASE[group] | 0x400);
    unsigned int pin = pins;
    dir &= (~pin);
    dir |= dirs;
    himm(GPIO_BASE[group] | 0x400, dir);
}

void SetGpioData(unsigned int group, unsigned int pin, unsigned int value)
{
    // unsigned int setPin = (0x04 << pin);
    // unsigned int setValue = (0 == value ? 0 : (0x01 << pin));
    // // printf("himm 0x%08X 0x%02X\n", GPIO_BASE[group] | setPin, setValue);
    // himm(GPIO_BASE[group] | setPin, setValue);

    unsigned int addr = GPIO_BASE[group];
    unsigned int offset = 0x0000 + (pin / 16) * 4;
    addr |= offset;
    // unsigned int iDir = dir;
    unsigned int m_value = getHimm(addr);
    m_value &= ~(0x01 << pin%16);
    m_value |= (0x01 << (16 + pin%16));
    m_value |= (value << pin%16);
    himm(addr, m_value);
}

void SetGpioDatas(unsigned int group, unsigned int pins, unsigned int value)
{
    unsigned int setPin = pins;
    setPin = (setPin << 2);
    himm(GPIO_BASE[group] | setPin, value);
}

unsigned int GetGpioData(unsigned int group, unsigned int pin)
{
    // unsigned int setPin = (0x04 << pin);
    // unsigned int value = getHimm(GPIO_BASE[group] | setPin);
    // value = value >> pin;
    // return value;

    unsigned int setPin = (0x04 << pin);
    unsigned int value = getHimm(GPIO_BASE[group] | 0x0000 + (pin / 16) * 4);
    value = value >> (pin % 16);
    return value & 0x01;
}

unsigned int GetGpioDatas(unsigned int group, unsigned int pins)
{
    unsigned int setPin = pins;
    setPin = (setPin << 2);
    return getHimm(GPIO_BASE[group] | setPin);
}

void himm(unsigned int addr, unsigned int value)
{
    void *pMem = memmap(addr, DEFAULT_MD_LEN);
    *(unsigned int *)pMem = value;
    memunmap(pMem);
    // cgpio *m = cgpio::Instance();
    // m->SetValue(addr, value);
}

unsigned int getHimm(unsigned int addr)
{
    void *pMem = memmap(addr, DEFAULT_MD_LEN);
    unsigned int ulOld = *(unsigned int *)pMem;
    memunmap(pMem);
    return ulOld;
    // cgpio *m = cgpio::Instance();
    // return m->GetValue(addr);
}
