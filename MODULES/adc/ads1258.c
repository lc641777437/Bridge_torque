#include "ads1258.h"
#include "spi.h"
#include "delay.h"
#include "exti.h"
#include "usart.h"
#include "sys.h"
#include "gpio.h"
#include "stmflash.h"

#define DeviceID *(vu32*)FALSH_SAVE_ADDR
static int ad_Data[16];
static int ad_Data_Max[16];
static int ad_Data_Min[16];
static long long int ad_Data_Sum[16];
static long int ad_Data_Num[16];
static char ad_State[16];


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

void send_AD_RawData(void)
{
    int i;
    ad_State[0]=STATE_0;
    ad_State[1]=STATE_1;
    ad_State[2]=STATE_2;
    ad_State[3]=STATE_3;
    ad_State[4]=STATE_4;
    ad_State[5]=STATE_5;
    ad_State[6]=STATE_6;
    ad_State[7]=STATE_7;
    ad_State[8]=STATE_8;
    ad_State[9]=STATE_9;
    ad_State[10]=STATE_10;
    ad_State[11]=STATE_11;
    ad_State[12]=STATE_12;
    ad_State[13]=STATE_13;
    ad_State[14]=STATE_14;
    ad_State[15]=STATE_15;
    for(i=0;i<16;i++)
    {
        if(ad_State[i]==0)
        {
            LOG_DEBUG("[DEVID:%d][%d]:%d\r\n",DeviceID,i+1,ad_Data[i]);
        }
    }
}



