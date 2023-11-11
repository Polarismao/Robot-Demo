#include "stm32f10x.h"
#include "ObjPublic.h"
#include "main.h"

extern void PhyWDG_Reload(void);
unsigned long baseTime=0;
unsigned short us_mainDelayTimer=0;
unsigned short us_pwmSet = 0;

int main(void)
{
    System_init();  //MCU系统初始化
    LgMain_Init();  //对象层模块初始化
    baseTime = GetTime();
    while(1)
    {
        if(baseTime-GetTime() >= TIME_500US)
        {
            baseTime -= TIME_500US;
            TimerDeal();
        }  

        if(bTimer2ms)
        {
            bTimer2ms=0;
            LgMain_2msDeal();
        }	
        PhyWDG_Reload();	 	
    }
}

/*************************************************************
*@brief【对象层模块初始化】
*@author mdq
*@date   2023-11-10
*@note 【备注】
*************************************************************/
void LgMain_Init(void)
{
    ObjPCA_Init();
	Iwdg_init();
}

/*************************************************************
*@brief【2ms时序】
*@author mdq
*@date   2023-11-10
*@note 【备注】
*************************************************************/
void LgMain_2msDeal(void)
{
    ObjPCA_servoAngleSet(15, us_pwmSet);
	switch(deal_step)
	{
		case 0:
		{
			object_232_deal();
			deal_step++;
			break;
		}
		case 1:
		{
			deal_step++;
			break;
		}
		case 2:
		{
            deal_step++;
			break;
		}								
		case 3:
		{
			deal_step++;
			break;
		}							
		case 4:
		{
			deal_step=0;
			break;
		}
		default:
		{
			deal_step=0;
			break;
		}			
	}	
}

/*************************************************************
*@brief【描述】
*@author mdq
*@date   2023-11-10
*@note 【备注】
*************************************************************/
void TimerDeal(void)
{   //软件定时器
	timerTicker++;
	if((timerTicker - timerTicker1msOld) >= 1U * 2) //1ms
	{
 		timerTicker1msOld = timerTicker;
   		bTimer1ms = 1;
	}

	if((timerTicker - timerTicker2msOld) >= 2U * 2) //2ms
	{
        timerTicker2msOld = timerTicker;
        bTimer2ms = 1;
	}
	bTimerHalfMs = 1;
}

