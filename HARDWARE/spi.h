#ifndef _SPI_H
#define _SPI_H

#include "stdint.h"
#include "stm32f10x.h"
#include "stm32f10x_spi.h"

void SPI_GPIO_Init(void);
uint8_t SPI2_WriteReadByte(uint8_t data);

#endif
