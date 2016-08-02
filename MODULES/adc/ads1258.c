#include "ads1258.h"
#include "spi.h"
#include "delay.h"
#include "exti.h"
#include "usart.h"
#include "sys.h"
#include "gpio.h"
#include "stmflash.h"
#include "fatfs_api.h"
#include "string.h"
#include "exfuns.h"



#define DeviceID *(vu32*)FALSH_SAVE_ADDR
static int ad_Data[16];
static int ad_Data_Max[16];
static int ad_Data_Min[16];
static long long int ad_Data_Sum[16];
static long int ad_Data_Num[16];
static u8 SendBuf[52];
int Date_Now;
char FileName[30];

void ads1258_Init(void)
{
    EXTI_Configuration();
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
    ad_Data[result[0]-8]&=0x00000000;
    ad_Data[result[0]-8]|=result[1];
    ad_Data[result[0]-8]<<=8;
    ad_Data[result[0]-8]|=result[2];
    ad_Data[result[0]-8]<<=8;
    ad_Data[result[0]-8]|=result[3];
    if(ad_Data[result[0]-8]>ad_Data_Max[result[0]-8])
        ad_Data_Max[result[0]-8]=ad_Data[result[0]-8];
    if(ad_Data[result[0]-8]<ad_Data_Min[result[0]-8])
        ad_Data_Min[result[0]-8]=ad_Data[result[0]-8];
    ad_Data_Sum[result[0]-8]+=ad_Data[result[0]-8];
    ad_Data_Num[result[0]-8]++;
}

void ads1258_ReadData(void)
{
    u8 iData[4];
    iData[0]=SPI2_ReadWriteByte(0xA0);
    iData[1]=SPI2_ReadWriteByte(0xA0);
    iData[2]=SPI2_ReadWriteByte(0xA0);
    iData[3]=SPI2_ReadWriteByte(0xA0);
    iData[0]&=0x1f;
    ad_DataConvert(iData);
}

void ad_Data_Proc(void)
{
    int n;
    int avr;
    for(n=0;n<16;n++)
    {
        avr=ad_Data_Sum[n]/ad_Data_Num[n];
        LOG_DEBUG("CHANNEL ID: %d\r\n",n);
        LOG_DEBUG("MAX: %d\r\n",ad_Data_Max[n]);
        LOG_DEBUG("MIN: %d\r\n",ad_Data_Min[n]);
        LOG_DEBUG("AVR: %d\r\n",avr);
        ad_Data_Sum[n]=0;
        ad_Data_Max[n]=0;
        ad_Data_Min[n]=0x7FFFFF;
        ad_Data_Num[n]=0;
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
    for(i=0;i<16;i++)
    {
        data_Buf = ad_Data[i];
        SendBuf[4 + 3 * i + 0] = data_Buf>>16;
        data_Buf = ad_Data[i];
        SendBuf[4 + 3 * i + 1] = (data_Buf&0xff00)>>8;
        data_Buf = ad_Data[i];
        SendBuf[4 + 3 * i + 2] = data_Buf&0xff;
    }
}

void Send_AD_RawData(u8 i)
{
    USART_SendData(USART1, (uint8_t)SendBuf[i]);
}


void Save_AD_RawData(void)
{
    RTC_DateTypeDef RTC_DateStruct;
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
    if(Date_Now!=RTC_DateStruct.RTC_Date)
    {
        mf_close();
        Date_Now=RTC_DateStruct.RTC_Date;
        snprintf(FileName,30,"%s%02d%02d%02d%s","0:/20",\
        RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date,".txt");
        mf_open((u8 *)FileName,FA_CREATE_NEW|FA_WRITE);
    }
//    else
//    {
//        //mf_open((u8 *)FileName,FA_WRITE);
//        //f_lseek(file,file->fsize);
//    }
    mf_write((u8 *)SendBuf,strlen((char *)SendBuf));//TODO:time
    mf_write("\r\n",2);
    //mf_close();
}

