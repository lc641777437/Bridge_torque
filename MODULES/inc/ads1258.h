#ifndef __ADS1258_H
#define __ADS1258_H

#include "stm32f4xx.h"

void ads1258_Init(void);
u8 ads1258_ReadRegister(u8 addr);
void ads1258_WriteRegister(u8 addr,u8 data);
void ads1258_ReadData(void);
		 
#endif


