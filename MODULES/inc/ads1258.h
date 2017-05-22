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
#define DATA_2_SIMCOM_LEN (148)//"CT" + 146
#define DATA_2_SIMCOM_DYNAMIC_LEN (54)//"DT" + 52
#define ADS1258_CHANNEL_NUM (16)

void ads1258_Init(void);
void ads1258_ReadData(void);

void upgrateBufferSended2Pc(void);
void upgrateBufferSended2Sim808(void);

u8 *ads1258_getSendData(void);
u8 *ads1258_getSendDataAvr(void);

void ads1258_SendDataBy232(void);// wired 232
void ads1258_SendDataBy808(void);// simcom
void ads1258_SendDataBy433(void);// wired 433

void Save_AD_RawData_SD(void);
void Save_AD_RawData_USB(void);

void ad_DataConvert(u8 result[4]);

void ads1258_SampleProc(void);

extern u8 DataSendPosition2PC;
extern u8 DataSendPosition2Simcom;

#endif/*__ADS1258_H__*/
