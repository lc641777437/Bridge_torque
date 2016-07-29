#include "main.h"


int main(void)
{
//    float time_s;
//    RTC_TimeTypeDef RTC_TimeStruct;
//    RTC_DateTypeDef RTC_DateStruct;
	SystemConfiguration();
    
    
	while(1)
	{
//        RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
//        time_s=(1023-RTC->SSR)/1024.f;
//        LOG_DEBUG("Time:%02d:%02d:%02d\r\n",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
//        LOG_DEBUG("%f\r\n",time_s);
//        RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
//        LOG_DEBUG("Date:20%02d-%02d-%02d\r\n",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date); 
        delay_ms(5050);        
	}
    
}

void SystemConfiguration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    UART_Init();
    TIM_Init();
    //SD_Card_Init();
    //LWIP_Init();
    ads1258_Init();
    My_RTC_Init();		 		//初始化RTC
    RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,9);		//配置WAKE UP中断,1秒钟中断一次
    RTC_Set_AlarmA(3,21,16,59);
    GPIO_init();
}


