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
	USART1_Configuration();
    USART2_Configuration();
	USART3_Configuration();
    mymem_init(SRAMIN);    //初始化内部内存池
    mymem_init(SRAMCCM);   //初始化CCM内存池
    check_SD_Card();
}


