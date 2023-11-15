#ifndef _PHYPUBLIC_H_
#define _PHYPUBLIC_H_
#include "stm32f10x.h"


GPIO_InitTypeDef		GPIO_InitStructure; 
CAN_InitTypeDef			CAN_InitStructure;
CAN_FilterInitTypeDef	CAN_FilterInitStructure;	
NVIC_InitTypeDef		NVIC_InitStructure;
GPIO_InitTypeDef		GPIO_InitStructure;
USART_InitTypeDef		USART_InitStructure;
SPI_InitTypeDef			SPI_InitStructure;
DMA_InitTypeDef			sEEDMA_InitStructure;
I2C_InitTypeDef			I2C_InitStructure;
DMA_InitTypeDef 		DMA_InitStructure;
CanRxMsg RxMessage;
CanTxMsg TxMessage;


extern __asm void PhySys_JumpToBoot(unsigned long SPaddr,unsigned long PCaddr);
extern void physical_can_send(unsigned long can_Id,unsigned char uc_exId, unsigned char can_data_lenth,unsigned char *can_data);

#endif


