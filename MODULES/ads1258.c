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

#define FILE_NAME_LEN (32)
#define DeviceID *(vu32*)FALSH_SAVE_ADDR

typedef enum{
    SAVE_NULL   = 0,
    SAVE_SD     = 1,
    SAVE_USB    = 2
} SAVE_POSITION;

static int ad_Data[ADS1258_CHANNEL_NUM];
static int ad_Data_Min[ADS1258_CHANNEL_NUM];
static int ad_Data_Max[ADS1258_CHANNEL_NUM];
static long int ad_Data_Num[ADS1258_CHANNEL_NUM];
static long long int ad_Data_Sum[ADS1258_CHANNEL_NUM];

static u8 SendBuf[DATA_2_PC_LEN]; // 52 Bytes to send 232
static u8 SendBuf_avr[DATA_2_SIMCOM_LEN]; // 148 Bytes to send to sim808

static SAVE_POSITION SavePosition = SAVE_NULL;  // 1 for SD 2 for USB

void ads1258_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
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

void ad_DataConvert(u8 result[4])
{
    ad_Data[result[0]-8] &= 0x00000000;
    ad_Data[result[0]-8] |= result[1];
    ad_Data[result[0]-8] <<= 8;
    ad_Data[result[0]-8] |= result[2];
    ad_Data[result[0]-8] <<= 8;
    ad_Data[result[0]-8] |= result[3];

    if(ad_Data[result[0]-8] > ad_Data_Max[result[0]-8])// store max value
    {
        ad_Data_Max[result[0]-8] = ad_Data[result[0]-8];
    }

    if(ad_Data[result[0]-8] < ad_Data_Min[result[0]-8] || ad_Data_Min[result[0]-8] == 0)// store min value
    {
        ad_Data_Min[result[0]-8] = ad_Data[result[0]-8];
    }

    ad_Data_Sum[result[0]-8] += ad_Data[result[0]-8];
    ad_Data_Num[result[0]-8]++;
}

void ads1258_ReadData(void)
{
    SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_TXE,ENABLE);
}

void ad_Data_Proc_Gprs(void)
{
    convert_AD_RawData_avr();
    ads1258_SendDataBy808();
}

void ads1258_SampleProc(void)
{
    static int time_10us = 0;
    if(time_10us < get_sampleTime()- 1)
    {
        time_10us++;
    }
    else
    {
        START = 1;
        time_10us=0;
    }
}

void convert_AD_RawData_avr(void)
{
    int i;
    int data_Buf;
    SendBuf_avr[0] = 'C';
    SendBuf_avr[1] = 'T';
    SendBuf_avr[2] = SendBuf[2];
    SendBuf_avr[3] = SendBuf[3];

    for(i = 0;i < ADS1258_CHANNEL_NUM; i++)
    {
        data_Buf = ad_Data_Max[i];
        SendBuf_avr[4 + 9 * i + 0] = data_Buf>>16;
        SendBuf_avr[4 + 9 * i + 1] = (data_Buf&0xff00)>>8;
        SendBuf_avr[4 + 9 * i + 2] = data_Buf&0xff;
        data_Buf = ad_Data_Min[i];
        SendBuf_avr[4 + 9 * i + 3] = data_Buf>>16;
        SendBuf_avr[4 + 9 * i + 4] = (data_Buf&0xff00)>>8;
        SendBuf_avr[4 + 9 * i + 5] = data_Buf&0xff;
        data_Buf = ad_Data_Sum[i]/ad_Data_Num[i];
        SendBuf_avr[4 + 9 * i + 6] = data_Buf>>16;
        SendBuf_avr[4 + 9 * i + 7] = (data_Buf&0xff00)>>8;
        SendBuf_avr[4 + 9 * i + 8] = data_Buf&0xff;
        ad_Data_Sum[i]=0;
        ad_Data_Max[i]=0;
        ad_Data_Min[i]=0;
        ad_Data_Num[i]=0;
    }
}

void convert_AD_RawData(void)
{
    int i;
    int data_Buf;
    u8 ad_State_send[2];
    SendBuf[0] = 0xA5;
    SendBuf[1] = 0xA5;
    ad_State_send[0]=STATE_0;
    ad_State_send[0]=(ad_State_send[0]<<1)|STATE_1;
    ad_State_send[0]=(ad_State_send[0]<<1)|STATE_2;
    ad_State_send[0]=(ad_State_send[0]<<1)|STATE_3;
    ad_State_send[0]=(ad_State_send[0]<<1)|STATE_4;
    ad_State_send[0]=(ad_State_send[0]<<1)|STATE_5;
    ad_State_send[0]=(ad_State_send[0]<<1)|STATE_6;
    ad_State_send[0]=(ad_State_send[0]<<1)|STATE_7;
    ad_State_send[1]=STATE_8;
    ad_State_send[1]=(ad_State_send[1]<<1)|STATE_9;
    ad_State_send[1]=(ad_State_send[1]<<1)|STATE_10;
    ad_State_send[1]=(ad_State_send[1]<<1)|STATE_11;
    ad_State_send[1]=(ad_State_send[1]<<1)|STATE_12;
    ad_State_send[1]=(ad_State_send[1]<<1)|STATE_13;
    ad_State_send[1]=(ad_State_send[1]<<1)|STATE_14;
    ad_State_send[1]=(ad_State_send[1]<<1)|STATE_15;
    SendBuf[2] = ad_State_send[0];
    SendBuf[3] = ad_State_send[1];
    for(i = 0;i < ADS1258_CHANNEL_NUM; i++)
    {
        data_Buf = ad_Data[i];
        SendBuf[4 + 3 * i + 0] = data_Buf>>16;
        data_Buf = ad_Data[i];
        SendBuf[4 + 3 * i + 1] = (data_Buf&0xff00)>>8;
        data_Buf = ad_Data[i];
        SendBuf[4 + 3 * i + 2] = data_Buf&0xff;
    }
}

void ads1258_SendDataBy232(void)// wired 232
{
    USART1_Send_Bytes(SendBuf, DATA_2_PC_LEN);
}

void ads1258_SendDataBy808(void)// simcom
{
    USART2_Send_Bytes(SendBuf_avr, DATA_2_SIMCOM_LEN);
}

void ads1258_SendDataBy433(void)// wireless 433
{
    USART3_Send_Bytes(SendBuf, DATA_2_PC_LEN);
}

void ads1258_SaveDataBySD(void)
{
    static int Date_Now_SD = 0;
    char FileName[FILE_NAME_LEN];
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;

    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
    if(Date_Now_SD != RTC_TimeStruct.RTC_Minutes)// 新的时间创建新的文件
    {
        mf_close();
        Date_Now_SD = RTC_TimeStruct.RTC_Minutes;
        SavePosition = SAVE_SD;
        snprintf(FileName, FILE_NAME_LEN, "%s%02d%02d%02d%02d%02d%02d%s", "0:/20",\
        RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month, RTC_DateStruct.RTC_Date,
        RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds, ".txt");
        mf_open((u8 *)FileName, FA_CREATE_NEW | FA_WRITE);
    }
    else if(SavePosition != SAVE_SD)//换了存储位置 打开原有文件
    {
        SavePosition = SAVE_SD;
        mf_close();
        snprintf(FileName, FILE_NAME_LEN, "%s%02d%02d%02d%02d%02d%02d%s", "0:/20",\
        RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month, RTC_DateStruct.RTC_Date,
        RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds, ".txt");
        mf_open((u8 *)FileName, FA_WRITE);
    }

    mf_write((u8 *)SendBuf, DATA_2_PC_LEN);
    mf_write("\r\n", 2);
}

void ads1258_SaveDataByUSB(void)
{
    static int Date_Now_USB = 0;
    char FileName[FILE_NAME_LEN];
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;

    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
    if(Date_Now_USB != RTC_TimeStruct.RTC_Minutes)// 新的时间创建新的文件
    {
        mf_close();
        Date_Now_USB = RTC_TimeStruct.RTC_Minutes;
        SavePosition = SAVE_USB;
        snprintf(FileName, FILE_NAME_LEN, "%s%02d%02d%02d%02d%02d%02d%s", "2:/20",\
        RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month, RTC_DateStruct.RTC_Date,
        RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds, ".txt");
        mf_open((u8 *)FileName, FA_CREATE_NEW | FA_WRITE);
    }
    else if(SavePosition != SAVE_USB)//换了存储位置 打开原有文件
    {
        SavePosition = SAVE_USB;
        mf_close();
        snprintf(FileName, FILE_NAME_LEN, "%s%02d%02d%02d%02d%02d%02d%s", "2:/20",\
        RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month, RTC_DateStruct.RTC_Date,
        RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds, ".txt");
        mf_open((u8 *)FileName, FA_WRITE);
    }

    mf_write((u8 *)SendBuf, DATA_2_PC_LEN);
    mf_write("\r\n", 2);
}
