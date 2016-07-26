#include "main.h"




//这是一个注释
int main(void)
{
	SystemConfiguration();
    while(lwip_comm_init()!=0)
	{
		LOG_DEBUG("lwIP Init failed!\r\n");
		delay_ms(1200);
		LOG_DEBUG("Retrying...\r\n"); 
	}
    LOG_DEBUG("lwIP Init Successed\r\n");
 	LOG_DEBUG("DHCP IP configing...\r\n");
    while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))
	{
		lwip_periodic_handle();
	}
    LOG_DEBUG("DHCP IP Successed\r\n");
    if(lwipdev.dhcpstatus==2)
    {
        LOG_DEBUG("DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
    }
	else 
    {
        LOG_DEBUG("Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
    }
	if(LAN8720_Get_Speed()&1<<1)
    {
        LOG_DEBUG("Ethernet Speed:100M");
    }
	else 
    {
        LOG_DEBUG("Ethernet Speed:10M");
    }
    httpd_init();
	while(1)
	{
        delay_ms(50);
	}
}

void SystemConfiguration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
	USART1_Configuration();//初始化串口1
    USART2_Configuration();//初始化串口2
	USART3_Configuration();//初始化串口3
    TIM3_Init();           //定时器初始化
    TIM4_Init();           //定时器初始化
    TIM5_Init();
    mymem_init(SRAMIN);    //初始化内部内存池
//    mymem_init(SRAMEX);    //初始化外部内存池
    mymem_init(SRAMCCM);   //初始化CCM内存池
//    check_SD_Card();       //初始化SD 卡
//    fatfs_init();          //初始化FATFS文件支持
}


