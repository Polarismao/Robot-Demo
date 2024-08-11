#include "PhyTimer.h"

/*************************************************
 * @brief 
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
void Timer_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE );   //使用APB1的开启时钟函数，因为TIM2位APB1总线的外设
    TIM_InternalClockConfig(TIM2);                         //选择内部时钟，一般默认就是内部可省

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;               //定义结构体类型
    TIM_TimeBaseInitStructure.TIM_ClockDivision= TIM_CKD_DIV1;       //1倍时钟分频，采样频率f可从内部时钟直接来，也可以内部时钟加个时钟分频而来
    TIM_TimeBaseInitStructure.TIM_CounterMode= TIM_CounterMode_Up;   //向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period= 10000-1;                   //周期即ARR自动重装器的值，计1000个数
    TIM_TimeBaseInitStructure.TIM_Prescaler= 7200-1;                 //PSC预分频器的值，差了个1原因见理论，对72MHz进行7200分频，即10KHz，计10000个数即1s
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter= 0 ;             //重复计数器的值，高级定时器才有，赋0
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);               //时基单元初始化

    TIM_ClearFlag(TIM2,TIM_FLAG_Update);                             //手动把更新中断标志位清除一下，解决刚初始化完就进入中断计数从1开始的问题
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel= TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd= ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority= 1;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2,ENABLE);
}


/*************************************************
 * @brief 
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-04
 * @copyright Copyright (c) 2024
 *************************************************/
void TIM2_IRQHandler(void)
{
    static unsigned char uc_timerCnt2 = 0;
    
    if(TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) == SET)
	{
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
		TIM2->CNT=0;
        uc_timerCnt2 ++;

        if(uc_timerCnt2 >= 5)
        {
            uc_timerCnt2 = 0;
            if(UsartStatusFlag==USART_SEND_OVER)    //发送完成且定时2ms后
            {	
                Usart_transTimer++;
                if(Usart_transTimer>=2)
                {
                    UsartStatusFlag = USART_TRANSITION;				      //发送完成开启接收定时
                    Usart_transTimer=0;
                    Usart_recev_len=0;
                    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);		  //开接收中断
                }
            }
            if(UsartStatusFlag==USART_RECEVING)  //从开启接收中断开始计时
            {
                Usart_receTimer++;
                if(Usart_receTimer > uc_uartRxOverNum)              //接收将持续2ms后，没有再接收数据会累计到4ms表示结束
                {
                    Usart_receTimer=0;
                    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); //关闭接收中断
                    UsartStatusFlag = USART_RECE_OVER;              //接收完成->对象层判断
                }	
            }	
        }
    }
}




