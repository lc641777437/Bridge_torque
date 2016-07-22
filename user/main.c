#include "main.h"


//这是一个注释
int main(void)
{
	SystemConfiguration();

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
    mymem_init(SRAMIN);    //初始化内部内存池
    mymem_init(SRAMCCM);   //初始化CCM内存池
    check_SD_Card();       //初始化SD 卡
    fatfs_init();          //初始化FATFS文件支持
}


