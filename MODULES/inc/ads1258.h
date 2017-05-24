/*
 * ads1258.h
 *
 *  Created on: 2017/05/18
 *      Author: lc
 */
#ifndef __ADS1258_H__
#define __ADS1258_H__

#include "stm32f4xx.h"

#define DATA_2_PC_LEN (52)// 0xa5 0xa5 + 50
#define DATA_2_SIMCOM_LEN (148)//"CT" + 146
#define DATA_2_SIMCOM_DYNAMIC_LEN (52)//"DT" + 50
#define ADS1258_CHANNEL_NUM (16)

void ads1258_Init(void);
void ads1258_ReadData(void);

void upgradeBufferSended2Pc(void);
void upgradeBufferSended2Sim808(void);
void upgradeBufferSended2Sim808Dynamic(void);

u8 *ads1258_getADValueSended2PC(void);
u8 *ads1258_getADValueSended2Server(void);
u8 *ads1258_getADValueSended2ServerDynamic(void);

void ads1258_SendDataBy232(void);// wired 232
void ads1258_SendDataBy433(void);// wired 433
void ads1258_SendDataBy808(void);// simcom
void ads1258_SendDynamicDataBy808(void);// simcom

void Save_AD_RawData_SD(void);
void Save_AD_RawData_USB(void);

void ads1258_storeOneChannelData(u8 result[4]);

void ads1258_SampleProc_10us(void);

extern u8 DataSendPosition2PC;
extern u8 DataSendPosition2Simcom;
extern u8 DataSendDyanamicPosition2Simcom;

#endif/*__ADS1258_H__*/
