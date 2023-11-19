#ifndef _PHYUART_H_
#define _PHYUART_H_

void USART_Config(unsigned char uart_nub,unsigned char uart_mode,unsigned char uart_Baut,unsigned char slave_mode);
void sci_uart_deal(unsigned char uart_nub,unsigned char uart_state,unsigned char *sci_uart_data,unsigned char sci_uart_lenth);
void PhyUart_ITConfig(unsigned char uart_nub,unsigned char uart_mode, unsigned char uart_enable);

#endif

