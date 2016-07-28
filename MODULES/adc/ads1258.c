#include "ads1258.h"
#include "spi.h"
#include "delay.h"
#include "exti.h"

static int ad_Data[16];

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




