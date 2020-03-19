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
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 |GPIO_Pin_8;		//������ӦС�ƹܽ�Ϊ�������,���תƵ��Ϊ50MHz | GPIO_Pin_4 | GPIO_Pin_5
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RS_L;
	Delay_us(100);
	
	SendCMD(0x30);//�������ã�һ����8λ���ݣ�����ָ�
	SendCMD(0x0C);//0000,1100 ������ʾ���α�off���α�λ��off
	SendCMD(0x01);//0000,0001 ��DDRAM
	SendCMD(0x02);//0000,0010 DDRAM��ַ��λ
	SendCMD(0x80);//1000,0000 �趨DDRAM 7λ��ַ000��0000����ַ������AC
}

void SendCMD(u8 dat){
	RS_H;
	SendByte(0xF8);
	SendByte(dat&0xF0);//����λ
	SendByte((dat&0x0F)<<4);//����λ
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
	SendCMD(x_add);//1xxx,xxxx �趨DDRAM 7λ��ַxxx,xxxx����ַ������AC
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
	SendByte(dat&0xF0);//����λ
	SendByte((dat&0x0F)<<4);//����λ
	RS_L;
} 




