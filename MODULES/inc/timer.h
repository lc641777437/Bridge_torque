#ifndef __TIMER_H
#define __TIMER_H

#include <string.h>
#include "stm32f4xx.h"

#define Sign_IN PFin(2)
#define Sign_OUT PFout(3)

#define NOTHING 0

#define SDCARD_OK 1
#define FATFS_OK 2

#define LWIP_OK 1
#define DHCP_OK 2
#define TCP_OK 3

#define USB_OK 1

void TIM_Init(void);

void TIM3_Init(void);
void TIM3_set(u8 sta);
void TIM4_Init(void);
void TIM4_set(u8 sta);
void TIM5_Init(void);
void TIM2_Init(void);
void TIM14_Init(void);
void TIM14_ENABLE(void);
    
void set_Frequent(int fre);

void TIM7_Init(void);
void TIM7_Enable(u8 state);
		 
#endif


