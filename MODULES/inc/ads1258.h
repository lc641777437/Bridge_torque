#ifndef __ADS1258_H
#define __ADS1258_H

#include "stm32f4xx.h"

void ads1258_Init(void);
u8 ads1258_ReadRegister(u8 addr);
void ads1258_WriteRegister(u8 addr,u8 data);
void ads1258_ReadData(void);
void ad_Data_Proc(void);
void send_AD_RawData(int i);
void Save_AD_RawData(void);

#endif


