#ifndef __IIC_H
#define __IIC_H

#include "stm32f10x.h"

// PB6 SCL PB7 SDA
 
#define I2C_SCL_Low  (GPIOB->BRR |= GPIO_Pin_6) //½« GPIOB_6ÖÃ0
#define I2C_SCL_High (GPIOB->BSRR |= GPIO_Pin_6)//½« GPIOB_6ÖÃ1
#define I2C_SDA_Low	 (GPIOB->BRR |= GPIO_Pin_7)
#define I2C_SDA_High (GPIOB->BSRR |= GPIO_Pin_7)

#define I2C_SDA_Read (GPIOB->IDR & GPIO_Pin_7)


void I2C_init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SetAck(FunctionalState ackState);
FunctionalState I2C_getAck(void);
FunctionalState I2C_WriteByte(u8 data);
u8 I2C_ReadByte(FunctionalState ackState);
u8 I2C_WriteByteToSlave(u8 addr , u8 reg , u8 data);
u8 I2C_WriteSomeDataToSlave(u8 addr, u8 reg,u8 len,u8 *buf);
u8 I2C_ReadFromSlave(u8 addr ,u8  reg , u8 *buf );
u8 I2C_ReadSomeDataFromSlave(u8 addr, u8 reg,u8 len,u8 *buf);
#endif
