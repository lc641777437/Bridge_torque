#include "main.h"

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
    UART_Init();
    TIM_Init();
    SD_Card_Init();
    LWIP_Init();
}


