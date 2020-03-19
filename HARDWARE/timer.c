#include "timer.h"

//定时器2 初始化
void Timer2_Init(uint16_t arr,uint16_t psc){
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//定义定时器结构体变量
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//使能TIM4的时钟
	
	//72的分频，一个时钟周期是 1微秒 ，一次中断是 1ms ，向上计数
	TIM_TimeBaseStructure.TIM_Period = arr-1;		//设置自动重装载的周期值
	TIM_TimeBaseStructure.TIM_Prescaler = psc-1;	//时钟频率 预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 			//定时器初始化函数
	
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);	//TIM2中断使能
	TIM_Cmd(TIM2, ENABLE);//TIM2使能
}
//定时器4 做PWM输出 
//void Timer4_Init(uint16_t arr,uint16_t psc){
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//定义定时器结构体变量
//	GPIO_InitTypeDef GPIO_InitStructure;//定义GPIO结构体变量
//	TIM_OCInitTypeDef  TIM_OCInitStructure;//定义输出比较结构体变量
//	//时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//使能TIM4的时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);//GPIOA、B、复用时钟使能
//	
//	//GPIO
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_9;//配置GPIO第8、9引脚
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//配置GPIO为复用推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//配置GPIO速率
//	GPIO_Init(GPIOB, &GPIO_InitStructure); //GPIO初始化函数
//	
//	//PWM周期是1ms = 1KHZ 
//	TIM_TimeBaseStructure.TIM_Period = arr-1;//设置自动重装载的周期值
//	TIM_TimeBaseStructure.TIM_Prescaler = psc-1;//设置预分频值
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//设置时钟分割
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数模式
//	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);//定时器初始化函数
//	
//	//OC：输出比较 
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM模式1
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
////	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;//设置了待装入捕获比较寄存器
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //极性为高 ， 当值小于  TIM_Pulse 为高电平 ， 大于为低电平
//	TIM_OC3Init(TIM4, &TIM_OCInitStructure);//比较输出初始化函数
//	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);//输出比较1预装载寄存器使能
//	
//	//通道 4：
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM模式1
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
////	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;//设置了待装入捕获比较寄存器
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //极性为高 ， 当值小于  TIM_Pulse 为高电平 ， 大于为低电平
//	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
//	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
//	
//	TIM_ARRPreloadConfig(TIM4, ENABLE);//使能或者失能 TIMx 在 ARR 上的预装载寄存器
//	TIM_Cmd(TIM4, ENABLE);//使能 TIME4
//	

//}

//	TIM_SetCompare1(TIM4,tempup3);



