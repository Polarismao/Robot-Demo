#include "PhyPublic.h"
#include "PhyUart.h"


/*************************************************************
*@brief【串口配置函数】
*@param  uart_nub    【串口号】
*@param  uart_mode   【串口模式】
*@param  uart_Baut   【参数注释】
*@param  slave_mode  【参数注释】
*@author mdq
*@date   2023-11-19
*@note 【备注】
*************************************************************/
void USART_Config(unsigned char uart_nub,unsigned char uart_mode,unsigned char uart_Baut,unsigned char slave_mode)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	USART_InitTypeDef	USART_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	if(uart_Baut==0)        USART_InitStructure.USART_BaudRate = 9600;
	else if(uart_Baut==1)	USART_InitStructure.USART_BaudRate = 19200;
	else if(uart_Baut==2)	USART_InitStructure.USART_BaudRate = 38400;
	else if(uart_Baut==3)	USART_InitStructure.USART_BaudRate = 57600;	
	else if(uart_Baut==4)	USART_InitStructure.USART_BaudRate = 115200;	
		
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	if(uart_mode==0) 		USART_InitStructure.USART_Mode = USART_Mode_Tx;
	else if(uart_mode==1)	USART_InitStructure.USART_Mode = USART_Mode_Rx;
	else if(uart_mode==2)	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
			
	switch(uart_nub)
	{
		case 1:
		{
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStructure);
		
			USART_Init(USART1, &USART_InitStructure);
			USART_Cmd(USART1, ENABLE);
			if(slave_mode==1)
			{
				USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
				GPIO_ResetBits(GPIOA, GPIO_Pin_1);
			}
			uc_uartRxOverNum = 2;

			break;
		}
		default:
		{
			break;
		}
	} 
}

/*************************************************************
*@brief【描述】
*@param  uart_nub    【参数注释】
*@param  uart_state  【参数注释】
*@param  sci_uart_data【参数注释】
*@param  sci_usart_lenth【参数注释】
*@author mdq
*@date   2023-11-19
*@note 【备注】
*************************************************************/
void Usart_Send_Deal(unsigned chara uart_nub,unsigned char uart_state,unsigned char *sci_uart_data,unsigned char sci_usart_lenth)
{
	unsigned char i;
	USART_TypeDef* uart_temp_ptr = 0;

	if(uart_state==0)
	{
		usart_lenth = sci_usart_lenth;
		for(i=0;i<sci_usart_lenth;i++)
		{
			Usart_trans_data[i] = *sci_uart_data++;			
		}
		usart_trans_len = 0;

		if(uart_nub==1)
			GPIO_SetBits(GPIOA, GPIO_Pin_1);

		USART_SendData(USART1, Usart_trans_data[usart_trans_len]); 	
		USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	}
	else if(uart_state==1)
	{
		Usart_recev_len = 0;
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	}
}

/*************************************************************
*@brief【描述】
*@param  uart_nub    【参数注释】
*@param  uart_mode   【参数注释】
*@param  uart_enable 【参数注释】
*@author mdq
*@date   2023-11-19
*@note 【备注】
*************************************************************/
void PhyUart_ITConfig(unsigned char uart_nub,unsigned char uart_mode, unsigned char uart_enable)
{
    USART_TypeDef * usart_periph = USART1;
		
    if(uart_nub == 1)
		usart_periph = USART1;
    else 
        return;

    if(uart_mode == 0)  //接受
    {
        if(uart_enable == 0)
        {
		    USART_ITConfig(usart_periph, USART_IT_RXNE, DISABLE);
        }
        else
        {
            USART_ITConfig(usart_periph, USART_IT_RXNE, ENABLE);
        }
    }
    else                //发送
    {
        if(uart_enable == 0)
        {
            USART_ITConfig(usart_periph, USART_IT_TC, DISABLE);
        }
        else
        {
            USART_ITConfig(usart_periph, USART_IT_TC, ENABLE);
        }
    }
}


/*************************************************************
*@brief【描述】
*@author mdq
*@date   2023-11-26
*@note 【备注】
*************************************************************/
void USART1_IRQHandler(void)
{     
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)    //接收数据 
	{
		Usart_recev_data[Usart_recev_len] = USART_ReceiveData(USART1);
		Usart_recev_len++;
		Usart_receTimer = 0;						 //接收时间清0
		UsartStatusFlag = USART_RECEVING;
		Usart_Timer = 0;
        
		if(Usart_recev_len > USART1_NOMAL_MAX)		 //错误，接收最多不能超过指定数据
		{ 
			Usart_recev_len = 0;
		}
	}
	else if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)   //发送数据
	{
		usart_trans_len += 1;
		Usart_Timer = 0;
		Usart_transTimer = 0;
		UsartStatusFlag = USART_SENDING;
		if(usart_trans_len >= usart_lenth)
		{
			USART_ITConfig(USART1, USART_IT_TC, DISABLE);		//关发送中断
			UsartStatusFlag = USART_SEND_OVER;				    //发送完成
			usart_trans_len = 0;
		}
		else
		{
			USART_SendData(USART1,Usart_trans_data[usart_trans_len]);
		}
	}
	else
	{
		uc_uartTempData = USART1->SR;
		uc_uartTempData = USART_ReceiveData(USART1);
		USART_ClearFlag(USART1,USART_FLAG_ORE|USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE|USART_FLAG_IDLE);				
	}
}