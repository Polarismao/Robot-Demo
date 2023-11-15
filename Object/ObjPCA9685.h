#ifndef _OBJPCA9685_H_
#define _OBJPCA9685_H_
#include "stm32f10x.h"


// 芯片地址
#define PCA9685_ADDR  0x80
// 芯片工作模式
#define PCA9685_MODE1 0x00
// 设置芯片分频
#define PCA9685_PRE   0xFE

#define LED0_ON_L    0x06
#define LED0_ON_H    0x07
#define LED0_OFF_L   0x08
#define LED0_OFF_H   0x09

void ObjPCA_ParaInit(void);
void ObjPCA_write(unsigned char uc_reg, unsigned char uc_data);
void ObjPCA_setFreq(float freq);
void ObjPCA_setPwm(unsigned char num, unsigned int on, unsigned int off);
void ObjPCA_servoAngleSet(unsigned char uc_servoNum,unsigned short us_servoAngle);
unsigned char ObjPCA_read(unsigned char reg);

#endif



