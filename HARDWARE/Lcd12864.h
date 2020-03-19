#ifndef __LCD12864_H
#define __LCD12864_H

#include "stm32f10x.h"

#define RS_H GPIO_SetBits(GPIOC,GPIO_Pin_8) //RS=1   4PIN
#define RS_L GPIO_ResetBits(GPIOC,GPIO_Pin_8) //RS=0

#define RW_H GPIO_SetBits(GPIOC,GPIO_Pin_7) //RW=1    5PIN
#define RW_L GPIO_ResetBits(GPIOC,GPIO_Pin_7);  //RW = 0

#define EN_H GPIO_SetBits(GPIOC,GPIO_Pin_6);    //EN = 1   6PIN
#define EN_L GPIO_ResetBits(GPIOC,GPIO_Pin_6);  //EN = 0


void Lcd12864_Init(void);
void SendCMD(u8 cmd);
void SendByte(unsigned char dat);
void SendDat(unsigned char dat);
void display(unsigned char x_add,unsigned char *ptr);
void delay(unsigned int n) ;
#endif
