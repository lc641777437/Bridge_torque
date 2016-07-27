#include "ads1258.h"
#include "spi.h"
#include "delay.h"
#include "exti.h"

int ad_Data[16];

void ads1258_Init(void)
{
    EXTI_Configuration();
    SPI2_Init();
    SPI2_ReadWriteByte(0xC0);
}

u8 ads1258_ReadRegister(u8 addr)
{
    u8 res;
    SPI2_ReadWriteByte(0x40|addr);
    res=SPI2_ReadWriteByte(0xA0);
    return res;
}

void ads1258_WriteRegister(u8 addr,u8 data)
{
    SPI2_ReadWriteByte(0x60|addr);
    SPI2_ReadWriteByte(data);
}

void ad_DataConvert(u8 result[4],int Data[16])
{
    Data[result[0]-8]&=0x00000000;
    Data[result[0]-8]|=result[1];
    Data[result[0]-8]<<=8;
    Data[result[0]-8]|=result[2];
    Data[result[0]-8]<<=8;
    Data[result[0]-8]|=result[3];
}

void ads1258_ReadData(void)
{
    u8 res[4];
    res[0]=SPI2_ReadWriteByte(0xA0);
    res[1]=SPI2_ReadWriteByte(0xA0);
    res[2]=SPI2_ReadWriteByte(0xA0);
    res[3]=SPI2_ReadWriteByte(0xA0);
    res[0]&=0x1f;
    ad_DataConvert(res,ad_Data);
}




