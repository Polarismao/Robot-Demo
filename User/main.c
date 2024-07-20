#include "stm32f10x.h"
#include "ObjPublic.h"
#include "main.h"

unsigned long baseTime=0;
unsigned short us_mainDelayTimer=0;
unsigned short us_pwmSet = 0;

int main(void)
{
    System_init();      //MCU系统初始化
    ObjModul_Init();    //对象层模块初始化
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
void ObjModul_Init(void)
{
    ObjPCA_ParaInit();
	ObjCan_ParaInit();
	Obj232_ParaInit();
	Delay_init();

}

/*************************************************************
*@brief【2ms时序】
*@author mdq
*@date   2023-11-10
*@note 【备注】
*************************************************************/
void LgMain_2msDeal(void)
{
    unsigned short us_step;
	ObjPCA_servoAngleSet(15, us_pwmSet);
	
	switch(us_step)
	{
		case 0:
		{
			ObjCan_MainDeal();
			Obj232_MainDeal();
			us_step++;
			break;
		}
		case 1:
		{
			us_step++;
			break;
		}
		case 2:
		{
            us_step++;
			break;
		}								
		case 3:
		{
			us_step++;
			break;
		}							
		case 4:
		{
			us_step=0;
			break;
		}
		default:
		{
			us_step=0;
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

