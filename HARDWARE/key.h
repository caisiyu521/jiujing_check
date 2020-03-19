#ifndef __KEY_H
#define __KEY_H

#include "sys.h"
#include "delay.h"
#include "stdio.h"

void Key_Init(void);
uint8_t keyScan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
char Key_Scan(void);
#endif

