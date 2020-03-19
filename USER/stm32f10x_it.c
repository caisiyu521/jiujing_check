#include "stm32f10x_it.h" 
#include "sys.h"
#include "led.h"
#include "adc.h"
#include "usart.h"
#include "oled.h"
#include "Network.h"

uint8_t beep_num = 0;


extern u8 caisiyu_flag ;
extern u8 nblot_flag ;
extern uchar  GPRS_Delay_1S ;

extern __IO uint16_t ADC_ConvertedValue;
float ADC_ConvertedValueLocal;    // 局部变量，用于保存转换计算后的电压值 
extern u8 MQ_flag_Time ;
uint16_t MQ_Buf = 0;
extern uint8_t string[20];
extern u8 JIEMIAN;


//TIM2中断服务函数
void TIM2_IRQHandler(void){	
	static int count1 ,count2 ,count3, i= 0,count4 ,count6;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET){//判断是否进入TIM2更新中断
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除TIM2更新中断
		if(++count1 == 1000){
			count1 = 0;
//			if(caisiyu_flag == 1)		{caisiyu_flag = 0;GPRS_Send_String("ok\r\n");}
//			else if(caisiyu_flag == 0)	{caisiyu_flag = 1;}
//			GPRS_Send_String("ok\r\n");
			LED0 = !LED0;
		}
		
		if(nblot_flag == 1){
			if(++count4 == 5000){
				count4 = 0;
				nblot_flag = 0;
			}
		}
		
		if(GPRS_Delay_1S == 1){					//精确延时1S
			if(++count6 == 100){
				count6 = 0;
				GPRS_Delay_1S = 0;
			}
		}
		
		if(beep_num){
			if(++count2 == 100){
				count2 = 0;
				BEEP = !BEEP;
				beep_num -= 1;
			}
		}
		else {BEEP = 0;}
		
		if(++count3 == 100){
			count3 = 0;
			if(JIEMIAN == 1){
				MQ_Buf += (float) ADC_ConvertedValue/4096*100; 
				i++;
				if(i%10 == 0){
					if(MQ_flag_Time != 0)  {MQ_flag_Time--;	beep_num = 2;}
					else if(MQ_flag_Time == 0)	JIEMIAN = 2;
				}
			}
			else if(JIEMIAN == 2){
				MQ_Buf = MQ_Buf/i;
				JIEMIAN = 3;
				OLED_CLS();
				printf("\r\n The current AD value = %d V \r\n",MQ_Buf); 
				i = 0;
			}
	
		}
	}	
}


#include "adc.h"
extern __IO uint16_t ADC_ConvertedValue;

void ADC_IRQHandler(void)
{	
	if (ADC_GetITStatus(ADCx,ADC_IT_EOC)==SET) 
	{
		// 读取ADC的转换值
		ADC_ConvertedValue = ADC_GetConversionValue(ADCx);
	}
	ADC_ClearITPendingBit(ADCx,ADC_IT_EOC);
}







