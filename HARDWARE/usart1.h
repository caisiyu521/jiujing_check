#ifndef __USART1_H
#define __USART1_H	 
#include "sys.h"   
#include "stdio.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   
 
#define USART1_MAX_RECV_LEN		800					//�����ջ����ֽ���
#define USART1_MAX_SEND_LEN		200					//����ͻ����ֽ���
#define USART1_RX_EN 			1					//0,������;1,����.

extern u8  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//���ջ���,���USART1_MAX_RECV_LEN�ֽ�
extern u8  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//���ͻ���,���USART1_MAX_SEND_LEN�ֽ�
extern u16 USART1_RX_STA;   						//��������״̬

void USART1_Init(u32 bound);				//����1��ʼ�� 
void TIM4_Set(u8 sta);
void TIM4_Init(u16 arr,u16 psc);
void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar);
void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u8 len);
void u1_printf(char* fmt, ...);
#endif




