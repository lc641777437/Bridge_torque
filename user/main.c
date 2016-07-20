#include "main.h"


//这是一个注释
int main(void)
{
	SystemConfiguration();
    while(SD_Init())//检测SD卡
	{
        LOG_DEBUG("%s","SD Card Error!");
		delay_ms(50);
        LOG_DEBUG("%s","Please Check!\r\n");
        delay_ms(50);
	}
    switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:LOG_DEBUG("Card Type:SDSC V1.1\r\n");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:LOG_DEBUG("Card Type:SDSC V2.0\r\n");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:LOG_DEBUG("Card Type:SDHC V2.0\r\n");break;
		case SDIO_MULTIMEDIA_CARD:LOG_DEBUG("Card Type:MMC Card\r\n");break;
	}	
    delay_ms(50);
  	LOG_DEBUG("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//制造商ID
    delay_ms(50);
 	LOG_DEBUG("Card RCA:%d\r\n",SDCardInfo.RCA);								//卡相对地址
    delay_ms(50);
	LOG_DEBUG("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));	//容量
    delay_ms(50);
 	LOG_DEBUG("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);			//块大小
	while(1)
	{
        //LOG_DEBUG("kky\r\n");
        delay_ms(50);
	}
}

void SystemConfiguration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
	USART1_Configuration();
    USART2_Configuration();
	USART3_Configuration();
    mymem_init(SRAMIN);    //初始化内部内存池
    mymem_init(SRAMCCM);   //初始化CCM内存池
}


