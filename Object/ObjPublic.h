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


/**************************�¼�֡*****************************/
#define	 FRM100IDE60_FDOOR		   		0
#define	 FRM100IDE61_FDOOR		   		1
#define	 FRM100IDE62_FDOOR		   		2
#define	 FRM100IDE63_FDOOR		   		3

#define  CF_INSERT_MAX1                 4

#define GetTime()		    ((SysTick->VAL) << 8)
#define SYSTEM_CLOCK_100    (7200000000)
#define TIME_1US            (72UL<<8)
#define TIME_500US          (TIME_1US * 500)
#define TIME_1MS            (TIME_1US * 1000)

extern unsigned short us_step;
extern unsigned short bTimerHalfMs;
extern unsigned short bTimer1ms;
extern unsigned short bTimer2ms;
extern unsigned short timerTicker;
extern unsigned short timerTickerHalfMsOld;
extern unsigned short timerTicker1msOld;
extern unsigned short timerTicker2msOld;

/*****************************************CANģ��*******************************************/
typedef struct
{
	unsigned long COB_ID;
	unsigned char uc_exId;
	unsigned char DLC;
	unsigned char Data[8];
	unsigned long ul_funId;
	unsigned short us_subId;
	unsigned short us_doorType;
	unsigned char uc_tgtAddr;
}CF_CAN;

struct CF_INSERT
{
	unsigned long ul_funId;
	unsigned short us_subId;
	unsigned char uc_exId;
	unsigned short us_tickLeft;
	unsigned short us_timeLimit;
	unsigned short us_timeCur;
	unsigned short us_waitPercent;
};

extern struct CF_INSERT st_cf_insertReq[CF_INSERT_MAX1];
extern CF_CAN st_cf_rcecFrame;
extern CF_CAN st_cf_sendFrame;

extern unsigned long ul_phyCan_rxId[4];
extern unsigned char uc_phyCan_exId[4];
extern unsigned char uc_phyCan_rxLen[4];
extern unsigned char uc_phyCan_rxMessage[4][8];	
extern unsigned char uc_phyCan_rxFlag[4];

void delay_ms(unsigned long ul_nms);
void delay_us(unsigned long ul_nus);

#endif

