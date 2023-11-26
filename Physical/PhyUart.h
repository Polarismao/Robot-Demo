#ifndef _PHYUART_H_
#define _PHYUART_H_

void USART_Config(unsigned char uart_nub,unsigned char uart_mode,unsigned char uart_Baut,unsigned char slave_mode);
void Usart_Send_Deal(unsigned char uart_nub,unsigned char uart_state,unsigned char *sci_uart_data,unsigned char sci_usart_lenth);
void PhyUart_ITConfig(unsigned char uart_nub,unsigned char uart_mode, unsigned char uart_enable);

#define USART1_NOMAL_MAX       50

#define USART_LEISURE           0   //0:表示空闲状态
#define USART_SENDING           1   //1:表示正在发送
#define USART_SEND_OVER         2   //2:表示发送完成
#define USART_RECEVING          3   //3:表示正在接收
#define USART_RECE_OVER         4   //4:表示接收完成
#define USART_TRANSITION        5   //5:表示发送完成和接收中间的状态


unsigned short Usart_transTimer;
unsigned short Usart_recev_len;
unsigned short Usart_receTimer;
unsigned short usart_trans_len;
unsigned short Usart_Timer;
unsigned short usart_lenth;
unsigned char uc_uartTempData;
unsigned char uc_uartRxOverNum;
unsigned char Usart_trans_data[70];
unsigned char Usart_recev_data[70];

unsigned short UsartStatusFlag;
//0:表示空闲状态
//1:表示正在发送
//2:表示发送完成
//3:表示正在接收
//4:表示接收完成
//5:表示发送完成和接收中间的状态

#endif

