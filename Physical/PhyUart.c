

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
			if(USART_InitStructure.USART_BaudRate >=38400)
				uc_uartRxOverNum[1] = 2;
			else
				uc_uartRxOverNum[1] = 5;
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
*@param  sci_uart_lenth【参数注释】
*@author mdq
*@date   2023-11-19
*@note 【备注】
*************************************************************/
void sci_uart_deal(unsigned char uart_nub,unsigned char uart_state,unsigned char *sci_uart_data,unsigned char sci_uart_lenth)
{
	unsigned char i;
	USART_TypeDef* uart_temp_ptr=0;
		
    if(uart_nub==1)	
	{
		uart_temp_ptr = USART2;	
	}
    
	if(uart_state==0)
	{
		uart_lenth[uart_nub]=sci_uart_lenth;
		for(i=0;i<sci_uart_lenth;i++)
		{
			uart_trans_data[uart_nub][i]=*sci_uart_data++;			
		}
		uart_trans_ptr[uart_nub] = 0;
		if(uart_nub==1)GPIO_SetBits(GPIOA, GPIO_Pin_1);
		else if(uart_nub==2)GPIO_SetBits(GPIOE, GPIO_Pin_15);
		USART_SendData(uart_temp_ptr, uart_trans_data[uart_nub][uart_trans_ptr[uart_nub]]); 	
		USART_ITConfig(uart_temp_ptr, USART_IT_TC, ENABLE);
	}
	else if(uart_state==1)
	{
		uart_recev_ptr[uart_nub] = 0;
		USART_ITConfig(uart_temp_ptr, USART_IT_RXNE, ENABLE);
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

