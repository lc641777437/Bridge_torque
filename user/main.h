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
#include "fatfs_api.h"
#include "exfuns.h"
#include "fatfs.h"


void SystemConfiguration(void);


#endif
