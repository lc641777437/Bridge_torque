#ifndef __ADS1258_H
#define __ADS1258_H

#include "stm32f4xx.h"

#define ADS1258_CHANNEL_NUM (16)

#define DATA_2_PC_LEN (52)
#define DATA_2_SIMCOM_LEN (148)

void ads1258_Init(void);
u8 ads1258_ReadRegister(u8 addr);
void ads1258_WriteRegister(u8 addr,u8 data);
void ads1258_ReadData(void);
void ad_Data_Proc_Gprs(void);
void convert_AD_RawData(void);
void convert_AD_RawData_avr(void);

void USART1_Send_AD_RawData(void);// wired 232
void USART2_Send_Data(void);// simcom
void USART3_Send_AD_RawData(void);// wired 433

void Save_AD_RawData_SD(void);
void Save_AD_RawData_USB(void);
void ad_DataConvert(u8 result[4]);

#endif


