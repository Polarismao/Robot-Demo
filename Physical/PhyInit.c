#include "stm32f10x.h"
#include "PhyIIC.h"
#include "PhyCan.h"

#define APP_VECT_TAB			NVIC_VectTab_FLASH
#define APP_VECT_TAB_OFFSET		0x0000

void System_Init(void);
void PhySys_InterruptInit(void);
void PhySys_EnableInterrupt(void);
void PhySys_DisableInterrupt(void);
void PhySys_ResetAllPeriph(void);
void RCC_Configuration(void);

void Iwdg_Init(void);
void PhyWDG_Reload(void);
void Delay_Init(void);
void Delay_ms(unsigned long ul_nms);
void Delay_us(unsigned long ul_nus);

unsigned long ul_fac_us;
unsigned long ul_fac_ms;

/*************************************************
 * @brief 系统初始化函数
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void System_Init(void)
{
 	PhySys_InterruptInit();	
	RCC_Configuration();			//配置系统时钟
	USART_Config(1,2,4,0);			//uart初始化
	CAN_Configuartion(1,0,0);		//CAN初始化
    I2C_Config();                   //IIC初始化
	Iwdg_Init();                    //看门狗初始化 
}


/*************************************************
 * @brief 中断初始化
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void PhySys_InterruptInit(void)
{
	PhySys_DisableInterrupt();
	PhySys_ResetAllPeriph();
	NVIC_SetVectorTable(APP_VECT_TAB,APP_VECT_TAB_OFFSET);	    //设置中断向量表地址	APP_VECT_TAB_OFFSET
	PhySys_EnableInterrupt();
}


/*************************************************
 * @brief 使能中断
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void PhySys_EnableInterrupt(void)
{
	__set_PRIMASK(0);
}


/*************************************************
 * @brief 关闭中断
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void PhySys_DisableInterrupt(void)
{
	__set_PRIMASK(1);
}


/*************************************************
 * @brief 复位所有外设
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void PhySys_ResetAllPeriph(void)
{
	RCC->APB1RSTR = 0xffffffff;
	RCC->APB2RSTR = 0xffffffff;
	RCC->APB1RSTR = 0;
	RCC->APB2RSTR = 0;

	SysTick->CTRL = 0;

	EXTI->IMR = 0;
	EXTI->EMR = 0;

	NVIC->ICER[0] = 0xffffffff;
	NVIC->ICER[1] = 0xffffffff;
	NVIC->ICER[2] = 0xffffffff;
	NVIC->ICER[3] = 0xffffffff;
	NVIC->ICER[4] = 0xffffffff;
	NVIC->ICER[5] = 0xffffffff;
	NVIC->ICER[6] = 0xffffffff;
	NVIC->ICER[7] = 0xffffffff;
}


/*************************************************
 * @brief 进入boot程序
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
__ASM void PhySys_JumpToBoot(unsigned long SPaddr,unsigned long PCaddr)
{		
	MOV SP,R0 		//初始化堆栈指针
	MOV R14,R1 		// 跳转到Reset_Handler
	BX LR 
}


/*************************************************
 * @brief 系统时钟初始化
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void RCC_Configuration(void)
{
	RCC_PCLK2Config(RCC_HCLK_Div1);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_USART1|RCC_APB2Periph_AFIO|RCC_APB2Periph_SPI1 , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_USART3|RCC_APB1Periph_UART4|RCC_APB1Periph_CAN1|RCC_APB1Periph_TIM2,ENABLE); 
}


/*************************************************
 * @brief 开门狗初始化
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void Iwdg_Init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);   //允许访问看门狗
	IWDG_SetPrescaler(IWDG_Prescaler_32);           //40K分频
	IWDG_SetReload(1250);                           //约为1S
	IWDG_ReloadCounter();
	IWDG_Enable();
}


/*************************************************
 * @brief 开门狗装载
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void PhyWDG_Reload(void)
{
    IWDG_ReloadCounter();//feed dog	
}


/*************************************************
 * @brief 延迟模块初始化
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void Delay_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	ul_fac_us = SystemCoreClock / 8000000;
	ul_fac_ms = fac_us * 1000;
}


/*************************************************
 * @brief ms级延迟函数
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void Delay_ms(unsigned long ul_nms)
{
	unsigned long temp;
	SysTick->LOAD = ul_fac_ms*ul_nms;
	SysTick->VAL=0X00;//清空计数器
	SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
	do
	{
		temp=SysTick->CTRL;//读取当前倒计数值
	}while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
	SysTick->CTRL=0x00; //关闭计数器
	SysTick->VAL =0X00; //清空计数器
}


/*************************************************
 * @brief us级延迟函数
 * @version 1.0
 * @author MAO (mao_deqiang@126.com)
 * @date 2024-08-11
 * @copyright Copyright (c) 2024
 *************************************************/
void Delay_us(unsigned long ul_nus)
{
	unsigned short us_temp;
	SysTick->LOAD = ul_fac_us*ul_nus;
	SysTick->VAL = 0X00;      //清空计数器
	SysTick->CTRL = 0X01;     //使能，减到零是无动作，采用外部时钟源
	do
	{
		us_temp = SysTick->CTRL;                     //读取当前倒计数值
	}while((us_temp&0x01) && (!(us_temp&(1<<16))));  //等待时间到达
	SysTick->CTRL = 0x00;                            //关闭计数器
	SysTick->VAL = 0X00;                             //清空计数器
}


