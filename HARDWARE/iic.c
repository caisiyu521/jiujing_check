#include "iic.h"
#include "delay.h"

//初始化 iic的 io口
void I2C_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//初始化 GPIOB
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
//从 从机中 读 多个 字节的数据
u8 I2C_ReadSomeDataFromSlave(u8 addr, u8 reg,u8 len,u8 *buf){
	FunctionalState state;
	I2C_Start();
	state = I2C_WriteByte(addr<<1|0);
	if(state == ENABLE){
		state = I2C_WriteByte(reg);
		if(state == ENABLE){
			I2C_Start();
			state = I2C_WriteByte(addr<<1|1);
			if(state == ENABLE){
				while(len){
					if(len == 1){
						*buf = I2C_ReadByte(DISABLE);
					}
					else {
						*buf = I2C_ReadByte(ENABLE);
					}
					len--;
					buf++;
				}
				I2C_Stop();
				return 0;
			}
		}
	}
	I2C_Stop(); 
	return 1;
}
//从从机中 读一个字节的数据
u8 I2C_ReadFromSlave(u8 addr ,u8  reg , u8 *buf ){
	FunctionalState state;
	I2C_Start();
	state = I2C_WriteByte(addr<<1|0);
	if(state == ENABLE){
		state = I2C_WriteByte(reg);
		if(state == ENABLE){
			I2C_Start();
			state = I2C_WriteByte(addr<<1|1);
			if(state == ENABLE){
				*buf = I2C_ReadByte(DISABLE);
				I2C_Stop();
				return 0;
			}
		}
	}
	I2C_Stop();
	return 1;
}
	
//向 从机中 写入 多个 字节
u8 I2C_WriteSomeDataToSlave(u8 addr, u8 reg,u8 len,u8 *buf){
	FunctionalState state;
	u8 i;
	I2C_Start();
	state = I2C_WriteByte(addr<<1|0);
	if(state == ENABLE){
		state = I2C_WriteByte(reg);
		if(state == ENABLE){
			for(i = 0;i < len;i++){
				state = I2C_WriteByte(*(buf+1));
				if(state == DISABLE){
					I2C_Stop();
					return 1;
				}
			}
			I2C_Stop();
			return 0;
		}
	}
	I2C_Stop();
	return 1;	
}

//往从机中的寄存器 写入 一个字节的数据
u8 I2C_WriteByteToSlave(u8 addr , u8 reg , u8 data){
	FunctionalState state;
	I2C_Start();
	state = I2C_WriteByte(addr<<1|0);
	if(state == ENABLE){
		state = I2C_WriteByte(reg);
		if(state == ENABLE){
			I2C_WriteByte(data);
			//正常写入 ，产生 停止信号
			I2C_Stop();
			return 0;
		}
	}
	I2C_Stop();
	return 1;
}

//读取 从机 发送的数据 ， 返回
u8 I2C_ReadByte(FunctionalState ackState){
	u8 i , data = 0x00;
	for(i = 0;i < 8;i++){
		I2C_SCL_High;	delay_us(1);
		data <<= 1;
		if(I2C_SDA_Read){
			data |= 0x01; 
		}
		I2C_SCL_Low;	delay_us(1);
	}
	I2C_SetAck(ackState);
	return data;
}

//写出数据给 从机，返回应答信号 或者 非应答信号
FunctionalState I2C_WriteByte(u8 data){
	u8 i;
	for(i = 0;i < 8;i++){
		I2C_SCL_Low;	delay_us(1);
		if(data & 0x80){
			I2C_SDA_High;	delay_us(1);
		}
		else {
			I2C_SDA_Low;	delay_us(1);
		}
		I2C_SCL_High;	delay_us(1);
		data <<= 1;		delay_us(1);
	}
	//主机释放SDA线 ， 使得总线空闲 ， 一遍MPu6050能发出响应，并牵住SCL线
	I2C_SCL_Low;	delay_us(1);
	I2C_SDA_High;	delay_us(1);
	
	return I2C_getAck(); 
}


void I2C_Start(void){
	I2C_SCL_Low;	delay_us(1);
	I2C_SDA_High;	delay_us(1);
	I2C_SCL_High;	delay_us(1);
	I2C_SDA_Low;	delay_us(1);
	//将 SCL 拉低 ，牵住SCL线，准备发数据
	I2C_SCL_Low;	delay_us(1);
}

void I2C_Stop(void){
	I2C_SCL_Low;	delay_us(1);
	I2C_SDA_Low;	delay_us(1);
	I2C_SCL_High;	delay_us(1);
	I2C_SDA_High;	delay_us(1);
//	I2C_SCL_Low;	delay_us(1);
}

//enable :主机发出应答信号 ，disable ：主机不发出应答信号
void I2C_SetAck(FunctionalState ackState){
	I2C_SCL_Low;	delay_us(1);
	if(ackState == ENABLE){
		I2C_SDA_Low;	delay_us(1);
	}
	else{
		I2C_SDA_High;	delay_us(1);
	}
	I2C_SCL_High;	delay_us(1);
	//释放掉 控制权
	I2C_SCL_Low;	delay_us(1);
	I2C_SDA_High;	delay_us(1);
}

//获取 应答信号 或者 非应答信号
FunctionalState I2C_getAck(void){
	FunctionalState ask;
	I2C_SCL_High;	delay_us(1);
	if(I2C_SDA_Read){
		ask = DISABLE;
	}
	else{
		ask = ENABLE;
	}
	I2C_SCL_Low;	delay_us(1);
	return ask;
}













