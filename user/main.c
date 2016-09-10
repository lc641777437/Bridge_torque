#include "main.h"

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

u8 USH_User_App(void)
{ 
	u32 total,free;
	u8 res=0;
	res=exf_getfree("2:",&total,&free);
	if(res==0)
	{
		LOG_DEBUG("U Disk Total Size:%dMB",total);	 
		LOG_DEBUG("U Disk  Free Size:%dMB",free); 	    
	} 
    mf_scan_files("2:");
	return res;
}

int main(void)
{
	SystemConfiguration();
    Write_String(0,0,"test");
    f_mount(fs[2],"2:",1); 	//挂载U盘
    USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks); 
    while(1)
    {
        USBH_Process(&USB_OTG_Core, &USB_Host);
        delay_ms(1);
    }
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
    UART_Init();
    TIM_Init();
    mymem_init(SRAMIN);
    mymem_init(SRAMCCM);
    exfuns_init();							//为fatfs相关变量申请内存
    SD_Card_Init();
    while(ETH_Mem_Malloc()){}		//申请内存
	while(lwip_comm_mem_malloc()){}	//申请内存
    LAN8720_Init();
    delay_ms(2000);
    LWIP_Init();
    ads1258_Init();
    My_RTC_Init();		 		//初始化RTC
    RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,9);      
    RTC_Set_AlarmA(1,0,0,0);
    GPIO_init();
    lcd12864_GPIO_Init();
    LCD_Init();
    //SysTick_Config(336000);
}


