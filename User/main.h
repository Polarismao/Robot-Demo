#ifndef _MAIN_H_
#define _MAIN_H_

#include "PCA9685.h"

void System_init(void);
void delay_ms(unsigned short nms);
void delay_us(unsigned int nus);

void System_init(void)
{
    ObjPCA_Init();
}

#endif
