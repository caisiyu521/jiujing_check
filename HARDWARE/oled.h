#ifndef __OLED_H
#define __OLED_H

#include "sys.h"
 
#define READ_SDA  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12)

#define IIC_SDA PAout(12)// PB5
#define IIC_SCL PAout(11)// PE5	

//#define LED2 PEout(2)// PB5
//#define LED3 PEout(3)// PE5	
//#define BEEP PFout(4)// PE5	

void IIC_Init(void);
void SDA_OUT(void);
void SDA_IN(void);
void IIC_Start(void);
void IIC_Stop(void);
u8 IIC_Wait_Ask(void);
void IIC_WriteByte(u8 data);
u8 IIC_ReadByte(void);
void WriteCmd(u8 command);
void WriteDat(u8 data);

void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_Init(void);
void OLED_ON(void);
void OLED_SetPos(unsigned char x, unsigned char y); //设置起始点坐标
void OLED_Fill(unsigned char fill_Data);//全屏填充
void OLED_CLS(void);//清屏
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);


#endif
