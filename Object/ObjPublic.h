#ifndef _OBJPUBLIC_H_
#define _OBJPUBLIC_H_

#include "ObjPCA9685.h"

#define CF_2MS    (2)
#define CF_5MS    (5)
#define CF_10MS   (10)
#define CF_20MS   (20)
#define CF_50MS   (50)
#define CF_100MS  (100)
#define CF_150MS  (150)
#define CF_200MS  (200)
#define CF_400MS  (400)
#define CF_500MS  (500)
#define CF_600MS  (600)
#define CF_650MS  (650)
#define CF_700MS  (700)
#define CF_800MS  (800)
#define CF_1S     (1000)
#define CF_1500MS (1500)
#define CF_2S     (2000)
#define CF_3S     (3000)
#define CF_4S     (4000)
#define CF_5S     (5000)
#define CF_6S     (6000)
#define CF_7S     (7000)
#define CF_10S    (10000)
#define CF_11S    (11000)
#define CF_12S    (12000)
#define CF_13S    (13000)
#define CF_14S    (14000)
#define CF_15S    (15000)
#define CF_20S    (20000) 
#define CF_30S    (30000) 
#define CF_1MIN   (60000)
#define CF_65S    (65000)
#define CF_3MINS  (180000)
#define CF_5MINS  (300000)
#define CF_10MINS (600000)
#define CF_24H    (86400000) //24H

#define GetTime()		    ((SysTick->VAL) << 8)
#define SYSTEM_CLOCK_100    (7200000000)
#define TIME_1US            (72UL<<8)
#define TIME_500US          (TIME_1US * 500)
#define TIME_1MS            (TIME_1US * 1000)

void delay_ms(unsigned short nms);
void delay_us(unsigned int nus);

/*****************************************CAN模块*******************************************/
typedef struct
{
	unsigned char  uc_exId;		  //增加扩展帧标记
	unsigned long  COB_ID;		  //功能帧
	unsigned char  Data[8];       //发送帧数据
	unsigned char  DLC;           //帧长度
	unsigned long  ul_funId;
	unsigned short us_subId;
    unsigned short us_landing;
    unsigned char  uc_doorFlag;
}CAN_FRAME2515;

#endif

