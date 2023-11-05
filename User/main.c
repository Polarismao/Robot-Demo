#include "stm32f10x.h"
#include "main.h"

unsigned short us_pwmSet = 0;

/*************************************************************
*@brief【系统初始化函数】
*@author mdq
*@date   2023-09-10
*@note 【备注】
*************************************************************/
void System_init(void)
{
    ObjPCA_Init();
}

int main(void)
{
    System_init();

    while(1)
    {
        ObjPCA_servoAngleSet(15, us_pwmSet);
    }
    
}


