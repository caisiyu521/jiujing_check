#include "Lcd12864.h"
#include "delay.h"

void Lcd12864_Clear(){
	SendCMD(0x01);
	Delay_ms(10);
}

//LED- GND
//LED+ VCC -5v
//PSB  GND
//E	   PC6
//RS   PC8
//RW   PC7
 
void Lcd12864_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 |GPIO_Pin_8;		//设置相应小灯管脚为推挽输出,最大翻转频率为50MHz | GPIO_Pin_4 | GPIO_Pin_5
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RS_L;
	Delay_us(100);
	
	SendCMD(0x30);//功能设置，一次送8位数据，基本指令集
	SendCMD(0x0C);//0000,1100 整体显示，游标off，游标位置off
	SendCMD(0x01);//0000,0001 清DDRAM
	SendCMD(0x02);//0000,0010 DDRAM地址归位
	SendCMD(0x80);//1000,0000 设定DDRAM 7位地址000，0000到地址计数器AC
}

void SendCMD(u8 dat){
	RS_H;
	SendByte(0xF8);
	SendByte(dat&0xF0);//高四位
	SendByte((dat&0x0F)<<4);//低四位
	RS_L;
}

void SendByte(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		EN_L;
		if(dat&0x80) 
			RW_H;
		else RW_L;
		EN_H;
		dat=dat<<1;
	}
}

void display(unsigned char x_add,unsigned char *ptr)
{
	SendCMD(x_add);//1xxx,xxxx 设定DDRAM 7位地址xxx,xxxx到地址计数器AC
	while(*ptr != '\0')
	{
		SendDat(*ptr);
		++ptr;
	}
}
void SendDat(unsigned char dat)
{
//      while(ReadBF){;}
	RS_H;
	SendByte(0xFA);//11111,01,0 RW=0,RS=1
	SendByte(dat&0xF0);//高四位
	SendByte((dat&0x0F)<<4);//低四位
	RS_L;
} 




