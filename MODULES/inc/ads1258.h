/*
 * ads1258.h
 *
 *  Created on: 2017/05/18
 *      Author: lc
 */
#ifndef __ADS1258_H__
#define __ADS1258_H__

#include "stm32f4xx.h"

#define DATA_2_PC_LEN (52)
#define DATA_2_SIMCOM_LEN (148)
#define ADS1258_CHANNEL_NUM (16)

void ads1258_Init(void);
void ads1258_ReadData(void);

void ad_Data_Proc_Gprs(void);
void convert_AD_RawData(void);
void convert_AD_RawData_avr(void);

void ads1258_SendDataBy232(void);// wired 232
void ads1258_SendDataBy808(void);// simcom
void ads1258_SendDataBy433(void);// wired 433

void ads1258_SaveDataBySD(void);
void ads1258_SaveDataByUSB(void);
void ad_DataConvert(u8 result[4]);

void ads1258_SampleProc(void);

#endif/*__ADS1258_H__*/
