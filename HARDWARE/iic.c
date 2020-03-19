#include "iic.h"
#include "delay.h"

//��ʼ�� iic�� io��
void I2C_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//��ʼ�� GPIOB
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
//�� �ӻ��� �� ��� �ֽڵ�����
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
//�Ӵӻ��� ��һ���ֽڵ�����
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
	
//�� �ӻ��� д�� ��� �ֽ�
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

//���ӻ��еļĴ��� д�� һ���ֽڵ�����
u8 I2C_WriteByteToSlave(u8 addr , u8 reg , u8 data){
	FunctionalState state;
	I2C_Start();
	state = I2C_WriteByte(addr<<1|0);
	if(state == ENABLE){
		state = I2C_WriteByte(reg);
		if(state == ENABLE){
			I2C_WriteByte(data);
			//����д�� ������ ֹͣ�ź�
			I2C_Stop();
			return 0;
		}
	}
	I2C_Stop();
	return 1;
}

//��ȡ �ӻ� ���͵����� �� ����
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

//д�����ݸ� �ӻ�������Ӧ���ź� ���� ��Ӧ���ź�
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
	//�����ͷ�SDA�� �� ʹ�����߿��� �� һ��MPu6050�ܷ�����Ӧ����ǣסSCL��
	I2C_SCL_Low;	delay_us(1);
	I2C_SDA_High;	delay_us(1);
	
	return I2C_getAck(); 
}


void I2C_Start(void){
	I2C_SCL_Low;	delay_us(1);
	I2C_SDA_High;	delay_us(1);
	I2C_SCL_High;	delay_us(1);
	I2C_SDA_Low;	delay_us(1);
	//�� SCL ���� ��ǣסSCL�ߣ�׼��������
	I2C_SCL_Low;	delay_us(1);
}

void I2C_Stop(void){
	I2C_SCL_Low;	delay_us(1);
	I2C_SDA_Low;	delay_us(1);
	I2C_SCL_High;	delay_us(1);
	I2C_SDA_High;	delay_us(1);
//	I2C_SCL_Low;	delay_us(1);
}

//enable :��������Ӧ���ź� ��disable ������������Ӧ���ź�
void I2C_SetAck(FunctionalState ackState){
	I2C_SCL_Low;	delay_us(1);
	if(ackState == ENABLE){
		I2C_SDA_Low;	delay_us(1);
	}
	else{
		I2C_SDA_High;	delay_us(1);
	}
	I2C_SCL_High;	delay_us(1);
	//�ͷŵ� ����Ȩ
	I2C_SCL_Low;	delay_us(1);
	I2C_SDA_High;	delay_us(1);
}

//��ȡ Ӧ���ź� ���� ��Ӧ���ź�
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













