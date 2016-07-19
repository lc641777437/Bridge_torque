#include "main.h"



int main(void)
{
	SystemConfiguration();
	while(1)
	{
	}
}

void SystemConfiguration(void)
{
	USART1_Configuration();
	USART3_Configuration();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}




