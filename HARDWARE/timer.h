#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_tim.h"

//��ʱ�� 2 ��ʼ��
void Timer2_Init(uint16_t arr,uint16_t psc);
//��ʱ�� 3 ��ʼ��
void Timer3_Init(void);
//��ʱ�� 4 ͨ�� 3,4 pwm���
void Timer4_Init(uint16_t arr,uint16_t psc);
#endif
