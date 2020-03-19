#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_tim.h"

//定时器 2 初始化
void Timer2_Init(uint16_t arr,uint16_t psc);
//定时器 3 初始化
void Timer3_Init(void);
//定时器 4 通道 3,4 pwm输出
void Timer4_Init(uint16_t arr,uint16_t psc);
#endif
