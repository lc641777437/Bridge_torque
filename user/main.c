/*
 * main.c
 *
 *  Created on: 2017/05/18
 *      Author: lc
 */
#include <stdio.h>
#include <string.h>

#include "stm32f4xx.h"

#include "sys.h"
#include "rtc.h"
#include "exti.h"
#include "gpio.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "malloc.h"
#include "exfuns.h"
#include "setting.h"
#include "ads1258.h"
#include "usbh_usr.h"
#include "stmflash.h"
#include "initstate.h"
#include "fatfs_api.h"
#include "sdio_sdcard.h"

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

static void SystemConfiguration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();

    GPIO_init();
    UART_Init();

    flash_restore();

    TIM_Init();
    EXTI_Configuration();

    mymem_init(SRAMIN);
    exfuns_init();//为fatfs相关变量申请内存
    SD_Card_Init();

    ads1258_Init();

    My_RTC_Init();//初始化RTC

    USB_Init();

    TIM14_IT_ENABLE();
}

int main(void)
{
	SystemConfiguration();

	while(1)
	{
        USBH_Process(&USB_OTG_Core, &USB_Host);

        if(get_Save_Flag())
        {
            if(get_DeviceState(DEVICE_USB) == ON && get_DeviceState(DEVICE_FATFS_USB) == ON)
            {
                set_Save_Flag(0);
                Save_AD_RawData_USB();
            }
            else if(get_DeviceState(DEVICE_SD) == ON && get_DeviceState(DEVICE_FATFS_SD) == ON)
            {
                set_Save_Flag(0);
                Save_AD_RawData_SD();
            }
        }
	}
}

