/*
 * ads1258.c
 *
 *  Created on: 2017/05/18
 *      Author: lc
 */
#include "spi.h"
#include "sys.h"
#include "exti.h"
#include "gpio.h"
#include "usart.h"
#include "string.h"
#include "exfuns.h"
#include "ads1258.h"
#include "setting.h"
#include "stmflash.h"
#include "fatfs_api.h"

u8 DataSendPosition2PC = 0;
u8 DataSendPosition2Simcom = 0;
u8 DataSendDyanamicPosition2Simcom = 0;

static int ADValue[ADS1258_CHANNEL_NUM];
static int ADValue_min[ADS1258_CHANNEL_NUM];
static int ADValue_max[ADS1258_CHANNEL_NUM];
static int ADValue_count[ADS1258_CHANNEL_NUM];
static long long ADValue_sum[ADS1258_CHANNEL_NUM];

static u8 ADValueSended2PC[DATA_2_PC_LEN];//  "\XA5\XA5" + 50 Bytes to send 232
static u8 ADValueSended2Server[DATA_2_SIMCOM_LEN];// "CT" + 146 Bytes to send to sim808
static u8 ADValueSended2ServerDynamic[DATA_2_SIMCOM_DYNAMIC_LEN];// "DT" + 50 Bytes to send sim808

void ads1258_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    START = 0;

    SPI2_Init();
    SPI2_ReadWriteByte(0xC0);
}

u8 ads1258_ReadRegister(u8 addr)
{
    SPI2_ReadWriteByte(0x40|addr);
    return SPI2_ReadWriteByte(0xA0);
}

void ads1258_WriteRegister(u8 addr,u8 data)
{
    SPI2_ReadWriteByte(0x60|addr);
    SPI2_ReadWriteByte(data);
}

void ads1258_ReadData(void)
{
    SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_TXE,ENABLE);
}

void ads1258_SampleProc_10us(void)
{
    static int time_10us = 0;

    if(time_10us < get_sampleTime()- 1)
    {
        time_10us++;
    }
    else
    {
        START = 1;
        time_10us = 0;
    }
}

void ads1258_storeOneChannelData(u8 result[4])
{
    ADValue[result[0]-8] &= 0x00000000;
    ADValue[result[0]-8] |= result[1];
    ADValue[result[0]-8] <<= 8;
    ADValue[result[0]-8] |= result[2];
    ADValue[result[0]-8] <<= 8;
    ADValue[result[0]-8] |= result[3];

    if(ADValue[result[0]-8] > ADValue_max[result[0]-8])// store max value
    {
        ADValue_max[result[0]-8] = ADValue[result[0]-8];
    }

    if(ADValue[result[0]-8] < ADValue_min[result[0]-8] || ADValue_min[result[0]-8] == 0)// store min value
    {
        ADValue_min[result[0]-8] = ADValue[result[0]-8];
    }

    ADValue_sum[result[0]-8] += ADValue[result[0]-8];
    ADValue_count[result[0]-8]++;
}

void upgradeBufferSended2Sim808(void)
{
    int i;
    u32 data;

    if(ADValue_count[0] == 0)return;

    ADValueSended2Server[0] = 'C';
    ADValueSended2Server[1] = 'T';
    ADValueSended2Server[2] = ADValueSended2PC[2];
    ADValueSended2Server[3] = ADValueSended2PC[3];

    for(i = 0;i < ADS1258_CHANNEL_NUM; i++)
    {
        data = ADValue_max[i];
        ADValueSended2Server[4 + 9 * i + 0] = data>>16;
        ADValueSended2Server[4 + 9 * i + 1] = (data&0xff00)>>8;
        ADValueSended2Server[4 + 9 * i + 2] = data&0xff;

        data = ADValue_min[i];
        ADValueSended2Server[4 + 9 * i + 3] = data>>16;
        ADValueSended2Server[4 + 9 * i + 4] = (data&0xff00)>>8;
        ADValueSended2Server[4 + 9 * i + 5] = data&0xff;

        data = ADValue_sum[i] / ADValue_count[i];
        ADValueSended2Server[4 + 9 * i + 6] = data>>16;
        ADValueSended2Server[4 + 9 * i + 7] = (data&0xff00)>>8;
        ADValueSended2Server[4 + 9 * i + 8] = data&0xff;

        ADValue_sum[i]=0;
        ADValue_max[i]=0;
        ADValue_min[i]=0;
        ADValue_count[i]=0;
    }
}

void upgradeBufferSended2Sim808Dynamic(void)
{
    int i;
    u32 data;

    if(ADValue_count[0] == 0)return;

    ADValueSended2ServerDynamic[0] = 'D';
    ADValueSended2ServerDynamic[1] = 'T';
    ADValueSended2ServerDynamic[2] = ADValueSended2PC[2];
    ADValueSended2ServerDynamic[3] = ADValueSended2PC[3];

    for(i = 0;i < ADS1258_CHANNEL_NUM; i++)
    {
        data = ADValue_sum[i] / ADValue_count[i];
        ADValueSended2ServerDynamic[4 + 3 * i + 0] = data>>16;
        ADValueSended2ServerDynamic[4 + 3 * i + 1] = (data&0xff00)>>8;
        ADValueSended2ServerDynamic[4 + 3 * i + 2] = data&0xff;

        ADValue_sum[i]=0;
        ADValue_max[i]=0;
        ADValue_min[i]=0;
        ADValue_count[i]=0;
    }
}


void upgradeBufferSended2Pc(void)
{
    int i = 0;
    u32 data = 0;
    u8 state[2] = {0, 0};

    state[0]=STATE_0;
    state[0]=(state[0]<<1)|STATE_1;
    state[0]=(state[0]<<1)|STATE_2;
    state[0]=(state[0]<<1)|STATE_3;
    state[0]=(state[0]<<1)|STATE_4;
    state[0]=(state[0]<<1)|STATE_5;
    state[0]=(state[0]<<1)|STATE_6;
    state[0]=(state[0]<<1)|STATE_7;
    state[1]=STATE_8;
    state[1]=(state[1]<<1)|STATE_9;
    state[1]=(state[1]<<1)|STATE_10;
    state[1]=(state[1]<<1)|STATE_11;
    state[1]=(state[1]<<1)|STATE_12;
    state[1]=(state[1]<<1)|STATE_13;
    state[1]=(state[1]<<1)|STATE_14;
    state[1]=(state[1]<<1)|STATE_15;

    ADValueSended2PC[0] = 0xA5;
    ADValueSended2PC[1] = 0xA5;
    ADValueSended2PC[2] = state[0];
    ADValueSended2PC[3] = state[1];
    for(i = 0;i < ADS1258_CHANNEL_NUM; i++)
    {
        data = ADValue[i];
        ADValueSended2PC[4 + 3 * i + 0] = data>>16;
        data = ADValue[i];
        ADValueSended2PC[4 + 3 * i + 1] = (data&0xff00)>>8;
        data = ADValue[i];
        ADValueSended2PC[4 + 3 * i + 2] = data&0xff;
    }
}

u8 *ads1258_getADValueSended2PC(void)
{
    return ADValueSended2PC;
}

u8 *ads1258_getADValueSended2Server(void)
{
    return ADValueSended2Server;
}

u8 *ads1258_getADValueSended2ServerDynamic(void)
{
    return ADValueSended2ServerDynamic;
}


void ads1258_SendDataBy232(void)// wired 232
{
    DataSendPosition2PC= 0;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);//open the Tx interrupt
}

void ads1258_SendDataBy433(void)// wireless 433
{
    DataSendPosition2PC = 0;
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);//open the Tx interrupt
}

void ads1258_SendDataBy808(void)// simcom
{
    if(get_isSendingType(SEND_NULL))
    {
        set_SendingType(SEND_CT);
        DataSendPosition2Simcom = 0;
    	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);//open the Tx interrupt
    }
}

void ads1258_SendDynamicDataBy808(void)// simcom
{
    if(get_isSendingType(SEND_NULL))
    {
        set_SendingType(SEND_DT);
        DataSendDyanamicPosition2Simcom = 0;
    	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);//open the Tx interrupt
    }
}

typedef enum{
    SAVE_NULL   = 0,
    SAVE_SD     = 1,
    SAVE_USB    = 2
} SAVE_POSITION;
#define FILE_NAME_LEN (32)
static long long timestampPre = 0;
static char FileName[FILE_NAME_LEN] = {0};
static SAVE_POSITION SavePosition = SAVE_NULL;
void Save_AD_RawData_SD(void)
{
    u32 timestampNow = get_timestamp();

    if(timestampNow - timestampPre >= TIME_FOR_SAVE)// 新的时间创建新的文件
    {
        SavePosition = SAVE_SD;
        timestampPre = timestampNow;

        mf_close();
        snprintf(FileName, FILE_NAME_LEN, "%s%d%s", "0:", timestampNow,".dat");
        mf_open((u8 *)FileName, FA_CREATE_NEW | FA_WRITE);
    }
    else if(SavePosition != SAVE_SD)//换了存储位置 打开原有文件
    {
        SavePosition = SAVE_SD;

        mf_close();
        snprintf(FileName, FILE_NAME_LEN, "%s%d%s", "0:", timestampNow,".dat");
        mf_open((u8 *)FileName, FA_WRITE);
    }

    mf_write((u8 *)ADValueSended2PC, DATA_2_PC_LEN);
    mf_write("\r\n", 2);
    return;
}

void Save_AD_RawData_USB(void)
{
    u32 timestampNow = get_timestamp();

    if(timestampNow - timestampPre >= TIME_FOR_SAVE)// 新的时间创建新的文件
    {
        SavePosition = SAVE_USB;
        timestampPre = timestampNow;

        mf_close();
        snprintf(FileName, FILE_NAME_LEN, "%s%d%s", "2:", timestampNow,".dat");
        mf_open((u8 *)FileName, FA_CREATE_NEW | FA_WRITE);
    }
    else if(SavePosition != SAVE_USB)//换了存储位置 打开原有文件
    {
        SavePosition = SAVE_USB;

        mf_close();
        snprintf(FileName, FILE_NAME_LEN, "%s%d%s", "2:", timestampNow,".dat");
        mf_open((u8 *)FileName, FA_WRITE);
    }

    mf_write((u8 *)ADValueSended2PC, DATA_2_PC_LEN);
    mf_write("\r\n", 2);
    return;
}

