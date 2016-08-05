#include "main.h"


int main(void)
{
	SystemConfiguration();
	while(1)
	{
        if(get_InitState(SDSTATE)==FATFS_OK)
        {
            if(get_Save_Flag()==1)
            {
                set_Save_Flag(0);
                Save_AD_RawData();
            }
        }
        lwip_periodic_handle();
	}
}

void SystemConfiguration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    UART_Init();
    TIM_Init();
    mymem_init(SRAMIN);
    mymem_init(SRAMCCM);
    SD_Card_Init();
    while(ETH_Mem_Malloc()){}		//申请内存
	while(lwip_comm_mem_malloc()){}	//申请内存
    LWIP_Init();
    ads1258_Init();
    My_RTC_Init();		 		//初始化RTC
    RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,9);      
    RTC_Set_AlarmA(1,0,0,0);
    GPIO_init();
    //SysTick_Config(336000);
}


