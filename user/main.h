#ifndef __MAIN_H__
#define __MAIN_H__



#include <stdio.h>
#include <string.h>


#include "stm32f4xx.h"
#include "delay.h"
#include "sys.h"
#include "spi.h"
#include "ads1258.h"
#include "usart.h"
#include "sdio_sdcard.h"
#include "malloc.h"
#include "exfuns.h"
#include "timer.h"
#include "exti.h"
#include "rtc.h"
#include "gpio.h"
#include "stmflash.h"
#include "device_state.h"
#include "lcd12864.h"
#include "usbh_usr.h"
#include "fatfs_api.h"

void SystemConfiguration(void);


#endif
