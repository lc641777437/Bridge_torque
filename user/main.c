#include "main.h"


int main(void)
{
	SystemConfiguration();
	while(1)
	{
        if(get_Save_Flag()==1)
        {
            set_Save_Flag(0);
            Save_AD_RawData();
        }       
	}
}

void SystemConfiguration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    UART_Init();
    TIM_Init();
    SD_Card_Init();
    //LWIP_Init();
    ads1258_Init();
    My_RTC_Init();		 		//≥ı ºªØRTC
    GPIO_init();
    //SysTick_Config(336000);
}


