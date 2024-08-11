#include "ObjPCA9685.h"
#include "PhyIIc.h"
#include "math.h"

/*************************************************
 * @brief 
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
void ObjPCA_ParaInit(void)
{
	ObjPCA_write(PCA9685_MODE1,0x00);
	ObjPCA_setFreq(50);       //设置PWM频率
    ObjPCA_servoAngleSet(15, 90);
	Delay_ms(500);
}


/*************************************************
 * @brief 
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
void ObjPCA_setFreq(float freq)
{
	unsigned char uc_prescale,uc_oldmode,uc_newmode;
	double prescaleval;

	freq *= 0.92; 
	prescaleval = 25000000;
	prescaleval /= 4096;
	prescaleval /= freq;
	prescaleval -= 1;
	uc_prescale = floor(prescaleval + 0.5f);
	
	uc_oldmode = ObjPCA_read(PCA9685_MODE1);
	uc_newmode = (uc_oldmode&0x7F) | 0x10;     //sleep

	ObjPCA_write(PCA9685_MODE1, uc_newmode);   //go to sleep
	ObjPCA_write(PCA9685_PRE, uc_prescale);    //set the prescaler
	ObjPCA_write(PCA9685_MODE1, uc_oldmode);
	Delay_ms(2);
	ObjPCA_write(PCA9685_MODE1, uc_oldmode | 0xA1);
}


/*************************************************
 * @brief 
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
void ObjPCA_setPwm(unsigned char num, unsigned int on, unsigned int off)
{
	ObjPCA_write(LED0_ON_L + 4*num, on);
	ObjPCA_write(LED0_ON_H + 4*num, on>>8);
	ObjPCA_write(LED0_OFF_L + 4*num, off);
	ObjPCA_write(LED0_OFF_H + 4*num, off>>8);
}


/*************************************************
 * @brief 给寄存器写入数据
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
void ObjPCA_write(unsigned char uc_reg, unsigned char uc_data)
{ 
	I2C_ByteWrite(uc_data, uc_reg);
}


/*************************************************
 * @brief 读取寄存器的数值
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
unsigned char ObjPCA_read(unsigned char reg)
{
	unsigned char uc_data;
	uc_data = I2C_ByteRead(reg);
	return uc_data;
}


/*************************************************
 * @brief 设置舵机的旋转角度
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
void ObjPCA_servoAngleSet(unsigned char uc_servoNum,unsigned short us_servoAngle)
{
	unsigned short pwm;
	pwm = (unsigned short)204.8*(0.5+(us_servoAngle*1.0/90));	      //4096/20ms=204.8/1ms 
				                                                      //(舵机驱动PWM周期是20ms,高低电平变化在0.5ms~2.5ms之间才能有效驱动舵机)
	ObjPCA_setPwm(uc_servoNum, 0, pwm);
}




