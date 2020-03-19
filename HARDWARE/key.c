#include "key.h"

void Key_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;  //��������
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_5 |GPIO_Pin_6| GPIO_Pin_7;  
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;  //�������
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7 |GPIO_Pin_8| GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_6);GPIO_SetBits(GPIOB,GPIO_Pin_7); GPIO_SetBits(GPIOB,GPIO_Pin_8); GPIO_SetBits(GPIOB,GPIO_Pin_9);  
}

/**
* �������ܣ����ּ��
* �� ��   ����
* ����ֵ  ���ɹ�-0  ʧ��-1
*/
uint8_t keyScan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{
        if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == 0)
        {
			while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == 0);  //���ּ��
			return 0;
        }
        else
            return 1;
}

/**
* �������ܣ��������
* �� ��   ����
* ����ֵ  ��int�ͼ�ֵ
*/
char Key_Scan(void)
{
        static u16 pin[4] = {GPIO_Pin_6 , GPIO_Pin_7 , GPIO_Pin_8, GPIO_Pin_9};
        static uint8_t i = 0;
        char temp = '&';
        
        GPIO_ResetBits(GPIOB,pin[i]);  //�����õ�
        
        if(keyScan(GPIOA,GPIO_Pin_4) == 0)  //�����һ���а�������
        {
                switch(i)
                {
                        case 0:return 'D';
                        case 1:return 'C';
                        case 2:return 'B';
                        case 3:return 'A';
                }
        }
        else if(keyScan(GPIOA,GPIO_Pin_5) == 0)   //����ڶ����а�������
        {
                switch(i)
                {
                        case 0:return '#';
                        case 1:return '9';
                        case 2:return '6';
                        case 3:return '3';
                }
        }
        else if(keyScan(GPIOA,GPIO_Pin_6) == 0)   //����������а�������
        {
                switch(i)
                {
                        case 0:return '0';
                        case 1:return '8';
                        case 2:return '5';
                        case 3:return '2';
                }
        }
        else if(keyScan(GPIOA,GPIO_Pin_7) == 0)   //����������а�������
        {
                switch(i)
                {
                        case 0:return '*';
                        case 1:return '7';
                        case 2:return '4';
                        case 3:return '1';
                }
        }
        GPIO_SetBits(GPIOB,pin[i]);  //�ø�
        
        i++;
                if(i >= 4)
                        i = 0;
        return temp;
}







