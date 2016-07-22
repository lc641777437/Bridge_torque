#ifndef __SPI_H
#define __SPI_H
#include "stm32f4xx.h"
   													  
void SPI1_Init(void);
void SPI1_SetSpeed(u8 SpeedSet);
u8 SPI1_ReadWriteByte(u8 TxData);

void SPI2_Init(void);
void SPI2_SetSpeed(u8 SpeedSet);
u8 SPI2_ReadWriteByte(u8 TxData);
		 
#endif



