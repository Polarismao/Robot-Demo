#include "PCA9685.h"
#include "PhyIIc.h"
#include "math.h"

/*************************************************************
*@brief【描述】
*@author mdq
*@date   2023-09-23
*@note 【备注】
*************************************************************/
void ObjPCA_Init(void)
{
	I2C_EE_Init();
	ObjPCA_write(PCA9685_MODE1,0x00);
	ObjPCA_setFreq(50);       //设置PWM频率
    ObjPCA_servoAngleSet(15, 90);    
	//delay_ms(500);
}

/*************************************************************
*@brief【描述】
*@param  freq        【参数注释】
*@author mdq
*@date   2023-09-23
*@note 【备注】
*************************************************************/
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

	uc_newmode = (uc_oldmode&0x7F) | 0x10; // sleep

	ObjPCA_write(PCA9685_MODE1, uc_newmode); // go to sleep

	ObjPCA_write(PCA9685_PRE, uc_prescale); // set the prescaler

	ObjPCA_write(PCA9685_MODE1, uc_oldmode);
	//delay_ms(2);

	ObjPCA_write(PCA9685_MODE1, uc_oldmode | 0xA1); 
}

/*************************************************************
*@brief【描述】
*@param  num         【参数注释】
*@param  on          【参数注释】
*@param  off         【参数注释】
*@author mdq
*@date   2023-09-23
*@note 【备注】
*************************************************************/
void ObjPCA_setPwm(unsigned char num, unsigned int on, unsigned int off)
{
	ObjPCA_write(LED0_ON_L + 4*num, on);
	ObjPCA_write(LED0_ON_H + 4*num, on>>8);
	ObjPCA_write(LED0_OFF_L + 4*num, off);
	ObjPCA_write(LED0_OFF_H + 4*num, off>>8);
}

/*************************************************************
*@brief【描述】
*@param  uc_reg      【参数注释】
*@param  uc_data     【参数注释】
*@author mdq
*@date   2023-09-23
*@note 【备注】
*************************************************************/
void ObjPCA_write(unsigned char uc_reg, unsigned char uc_data)
{ 
	I2C_ByteWrite(uc_data, uc_reg);
}

/*************************************************************
*@brief【描述】
*@param  reg         【参数注释】
*@return unsigned char【返回值注释】
*@author mdq
*@date   2023-09-23
*@note 【备注】
*************************************************************/
unsigned char ObjPCA_read(unsigned char reg)
{
	unsigned char uc_data;
	uc_data = I2C_ByteRead(reg);
	return uc_data;
}

/*************************************************************
*@brief【描述】
*@param  uc_servoNum 【参数注释】
*@param  us_servoAngle【参数注释】
*@author mdq
*@date   2023-09-23
*@note 【备注】
*************************************************************/
void ObjPCA_servoAngleSet(unsigned char uc_servoNum,unsigned short us_servoAngle)
{
	unsigned short pwm;
	pwm = (unsigned short)204.8*(0.5+(us_servoAngle*1.0/90));	      //4096/20ms=204.8/1ms 
				                                                      //(舵机驱动PWM周期是20ms,高低电平变化在0.5ms~2.5ms之间才能有效驱动舵机)
	ObjPCA_setPwm(uc_servoNum, 0, pwm);
}


