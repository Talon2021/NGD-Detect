#ifndef _GPIO_H_
#define _GPIO_H_

#define MAX_LENGTH  10*1024*1024
#ifndef DEFAULT_MD_LEN
    #define DEFAULT_MD_LEN 128
#endif
void himm(unsigned int addr, unsigned int value);
unsigned int getHimm(unsigned int addr);

// pin - 管脚, 0~7; dir - 方向, 1-输出,程序写数据, 0-输入, 程序读数据 
void SetGpioDir(unsigned int group, unsigned int pin, unsigned int dir);
unsigned int GetGpioDir(unsigned int group, unsigned int pin);
// pins - 8位, 置1表示此管脚方向需要配置, dirs - 使用对应位的方向值
void SetGpioGroupDirs(unsigned int group, unsigned int pins, unsigned int dirs);
void SetGpioData(unsigned int group, unsigned int pin, unsigned int value);
void SetGpioDatas(unsigned int group, unsigned int pins, unsigned int value);
unsigned int GetGpioData(unsigned int group, unsigned int pin);
unsigned int GetGpioDatas(unsigned int group, unsigned int pins);

#endif //_GPIO_H_