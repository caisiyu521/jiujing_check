#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
//extern u8  USART1_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART1_RX_STA;         		//接收状态标记	
extern u8  USART2_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART2_RX_STA;         		//接收状态标记	
extern u8  USART3_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART3_RX_STA;         		//接收状态标记	
extern u16 len ,t;
// printf() 只是对 Usrat1 的 ， 
//void uart1_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);
void Usart_SendChar(USART_TypeDef * pUSARTx,  uint8_t ch);
void Usart_SendString(USART_TypeDef * pUSARTx, char *str);
#define USART2_TxPin GPIO_Pin_2
#define USART2_RxPin GPIO_Pin_3
void sendByte(USART_TypeDef *USARTx, u16 byte);
void sendString(USART_TypeDef *USARTx, char *str);
void Usart_SendChar(USART_TypeDef * pUSARTx,  uint8_t ch);
void Usart_SendString(USART_TypeDef * pUSARTx,char *str);

#endif


