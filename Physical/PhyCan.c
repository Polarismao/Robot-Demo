#include "stm32f10x.h"
#include "PhyCan.h"
#include "PhyPublic.h"

/*************************************************************
*@brief【CAN模块配置】
*@param  can_baut    【波特率】
*@param  Acr         【参数注释】
*@param  Amr         【参数注释】
*@author mdq
*@date   2023-11-09
*@note 【备注】
*************************************************************/
void CAN_Configuartion(unsigned char can_baut, unsigned short Acr, unsigned short Amr)
{									 
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn; //CAN
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	CAN_InitStructure.CAN_TTCM=DISABLE;
	CAN_InitStructure.CAN_ABOM=ENABLE;
	CAN_InitStructure.CAN_AWUM=DISABLE;
	CAN_InitStructure.CAN_NART=DISABLE;
	CAN_InitStructure.CAN_RFLM=DISABLE;
	CAN_InitStructure.CAN_TXFP=DISABLE;
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;// 正常模式
	if(can_baut==0)
	{
		CAN_InitStructure.CAN_SJW=CAN_SJW_3tq;  //计算波特率
		CAN_InitStructure.CAN_BS1=CAN_BS1_13tq;
		CAN_InitStructure.CAN_BS2=CAN_BS2_6tq;
		CAN_InitStructure.CAN_Prescaler=36;  //50k
	}
	else if(can_baut==1)//100k
	{
		CAN_InitStructure.CAN_SJW=CAN_SJW_3tq;  //计算波特率
		CAN_InitStructure.CAN_BS1=CAN_BS1_13tq;
		CAN_InitStructure.CAN_BS2=CAN_BS2_6tq;
		CAN_InitStructure.CAN_Prescaler=18;  //100k			
	}
	else if(can_baut==2)//250k
	{
		CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;  //计算波特率
		CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;
		CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;
		CAN_InitStructure.CAN_Prescaler = 24;	
		
	}	
	CAN_Init(CAN1,&CAN_InitStructure);
	
	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber=0;
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh=Acr<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=(~Amr)<<5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0; 			//选择接收FIFO
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);
}

/*************************************************************
*@brief【CAN物理层发送函数】
*@param  can_Id      【参数注释】
*@param  uc_exId     【参数注释】
*@param  can_data_lenth【参数注释】
*@param  can_data    【参数注释】
*@author mdq
*@date   2023-11-09
*@note 【备注】
*************************************************************/
void physical_can_send(unsigned long can_Id,unsigned char uc_exId, unsigned char can_data_lenth,unsigned char *can_data)
{
	unsigned char i;
	unsigned char *ptr;
    
	TxMessage.StdId = can_Id&0x7ff;         //11位标准帧
	TxMessage.ExtId = can_Id&0x1fffffff;    //29位扩展帧

    if(0 == uc_exId)
        TxMessage.IDE = CAN_ID_STD;
    else
        TxMessage.IDE = CAN_ID_EXT;

	TxMessage.RTR = CAN_RTR_DATA;   //数据帧
	TxMessage.DLC = can_data_lenth; //发送长度

	ptr = can_data; 
	for(i=0;i<can_data_lenth;i++)
	{	
		TxMessage.Data[i]=*ptr++;  //传递数据
	}
	ptr =can_data; 
	for(i=0;i<can_data_lenth;i++)     //清除发送数据
	{
		*ptr++=0;
	}	
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE); 
	CAN_Transmit(CAN1,&TxMessage);   //发送数据
	 	
}

/*************************************************************
*@brief【CAN接收中断】
*@author mdq
*@date   2023-11-09
*@note 【备注】
*************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	unsigned char i,m;	
	RxMessage.StdId=0x00;
	RxMessage.ExtId=0x00;
	RxMessage.IDE=0;
	RxMessage.DLC=0;
	RxMessage.FMI=0;
	for(i=0;i<8;i++)
	{
		RxMessage.Data[i]=0x00; //清除其内容
	}
	CAN_Receive(CAN1,CAN_FIFO0, &RxMessage);
	for(m=0;m<4;m++)	
	{
		if(uc_phyCan_rxFlag[m]==0)
		{
		    if(CAN_ID_STD == RxMessage.IDE)
		    {
		        uc_phyCan_exId[m] = 0;
    			ul_phyCan_rxId[m] = RxMessage.StdId;
		    }
            else
            {
		        uc_phyCan_exId[m] = 1;
    			ul_phyCan_rxId[m] = RxMessage.ExtId;                
            }
            
			uc_phyCan_rxLen[m] = RxMessage.DLC;
			for(i=0;i<8;i++)
			{
				uc_phyCan_rxMessage[m][i]=RxMessage.Data[i];
			}
			uc_phyCan_rxFlag[m] = 1;
			break;
		}	
	}
}

