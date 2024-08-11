#ifndef _MAIN_H_
#define _MAIN_H_

extern void System_Init(void);
extern void Iwdg_Init(void);
extern void PhyWDG_Reload(void);

void ObjModul_Init(void);
void LgMain_2msDeal(void);
void TimerDeal(void);


unsigned short bTimerHalfMs;
unsigned short bTimer1ms;
unsigned short bTimer2ms;
unsigned short timerTicker;
unsigned short timerTickerHalfMsOld;
unsigned short timerTicker1msOld;
unsigned short timerTicker2msOld;


#endif
