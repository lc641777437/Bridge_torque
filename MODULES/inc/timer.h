#ifndef __TIMER_H
#define __TIMER_H

#include <string.h>
#include "stm32f4xx.h"

void TIM_Init(void);

void TIM3_Init(void);
void TIM3_set(u8 sta);
void TIM4_Init(void);
void TIM4_set(u8 sta);
void TIM5_Init(void);
void TIM2_Init(void);
void TIM14_Init(void);
void set_Frequent(int fre);
		 
#endif


