#include "Motor-28BYJ-48 .h"

/* 
	PC6-PC9 
*/
//ËÄÏà °ËÅÄ 
//0x01,0x03,0x02,0x06,0x04,0x0c,0x08,0x09
long int pulse[8] = {0x03800040,0x030000c0,0x03400080,0x02400180,0x02c00100,0x00c00300,0x01c00200,0x01800240};

void moto_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOC,GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);
}
//Çý¶¯
//for(j = 200;j > 0; j--){
//			for(i = 0;i < 8;i++){
//					GPIOC->BSRR = pulse[i];
//					Delay_ms(5);
//			}
//}
