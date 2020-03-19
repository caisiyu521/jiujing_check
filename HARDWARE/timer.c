#include "timer.h"

//��ʱ��2 ��ʼ��
void Timer2_Init(uint16_t arr,uint16_t psc){
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//���嶨ʱ���ṹ�����
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ʹ��TIM4��ʱ��
	
	//72�ķ�Ƶ��һ��ʱ�������� 1΢�� ��һ���ж��� 1ms �����ϼ���
	TIM_TimeBaseStructure.TIM_Period = arr-1;		//�����Զ���װ�ص�����ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc-1;	//ʱ��Ƶ�� Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 			//��ʱ����ʼ������
	
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);	//TIM2�ж�ʹ��
	TIM_Cmd(TIM2, ENABLE);//TIM2ʹ��
}
//��ʱ��4 ��PWM��� 
//void Timer4_Init(uint16_t arr,uint16_t psc){
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//���嶨ʱ���ṹ�����
//	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO�ṹ�����
//	TIM_OCInitTypeDef  TIM_OCInitStructure;//��������ȽϽṹ�����
//	//ʱ��
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//ʹ��TIM4��ʱ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);//GPIOA��B������ʱ��ʹ��
//	
//	//GPIO
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_9;//����GPIO��8��9����
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//����GPIOΪ�����������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//����GPIO����
//	GPIO_Init(GPIOB, &GPIO_InitStructure); //GPIO��ʼ������
//	
//	//PWM������1ms = 1KHZ 
//	TIM_TimeBaseStructure.TIM_Period = arr-1;//�����Զ���װ�ص�����ֵ
//	TIM_TimeBaseStructure.TIM_Prescaler = psc-1;//����Ԥ��Ƶֵ
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//����ʱ�ӷָ�
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ���ģʽ
//	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);//��ʱ����ʼ������
//	
//	//OC������Ƚ� 
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWMģʽ1
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
////	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;//�����˴�װ�벶��ȽϼĴ���
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //����Ϊ�� �� ��ֵС��  TIM_Pulse Ϊ�ߵ�ƽ �� ����Ϊ�͵�ƽ
//	TIM_OC3Init(TIM4, &TIM_OCInitStructure);//�Ƚ������ʼ������
//	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);//����Ƚ�1Ԥװ�ؼĴ���ʹ��
//	
//	//ͨ�� 4��
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWMģʽ1
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
////	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;//�����˴�װ�벶��ȽϼĴ���
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //����Ϊ�� �� ��ֵС��  TIM_Pulse Ϊ�ߵ�ƽ �� ����Ϊ�͵�ƽ
//	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
//	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
//	
//	TIM_ARRPreloadConfig(TIM4, ENABLE);//ʹ�ܻ���ʧ�� TIMx �� ARR �ϵ�Ԥװ�ؼĴ���
//	TIM_Cmd(TIM4, ENABLE);//ʹ�� TIME4
//	

//}

//	TIM_SetCompare1(TIM4,tempup3);



