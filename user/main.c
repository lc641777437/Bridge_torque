#include "main.h"

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

int main(void)
{
	SystemConfiguration();
	while(1)
	{
        USBH_Process(&USB_OTG_Core, &USB_Host);
        if(get_InitState(USBSTATE)==USB_OK)
        {
            if(get_Save_Flag()==1)
            {
                set_Save_Flag(0);
                Save_AD_RawData_USB();
            }
        }
        else if(get_InitState(SDSTATE)==FATFS_OK)
        {
            if(get_Save_Flag()==1)
            {
                set_Save_Flag(0);
                Save_AD_RawData_SD();
            }
        }
        if(get_InitState(ETHSTATE)==TCP_OK)
        {
            lwip_periodic_handle();
        }
	}
}

void SystemConfiguration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    GPIO_init();
    UART_Init();
    TIM_Init();
    My_Flash_Read();
    mymem_init(SRAMIN);
    mymem_init(SRAMCCM);
    exfuns_init();							//为fatfs相关变量申请内存
    SD_Card_Init();
    while(ETH_Mem_Malloc()){}		//申请内存
	while(lwip_comm_mem_malloc()){}	//申请内存
    LAN8720_Init();
    delay_ms(500);
    LWIP_Init();
    ads1258_Init();
    My_RTC_Init();		 		//初始化RTC
    RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,9);      
    RTC_Set_AlarmA(1,0,0,0);
    lcd12864_GPIO_Init();
    LCD_Init();
    USB_Init();
}


