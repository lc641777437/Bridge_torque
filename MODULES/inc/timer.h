/*
 * timer.h
 *
 *  Created on: 2017/05/18
 *      Author: lc
 */
#ifndef __TIMER_H__
#define __TIMER_H__

#include <string.h>
#include "stm32f4xx.h"

#define Sign_IN PFin(2)
#define Sign_OUT PFout(3)

void TIM_Init(void);

void TIM2_Init(void);
void TIM3_Init(void);
void TIM3_set(u8 sta);
void TIM4_Init(void);
void TIM4_set(u8 sta);
void TIM5_Init(void);
void TIM7_Init(void);
void TIM7_Enable(u8 state);
void TIM13_Init(void);
void TIM13_Enable(u8 state);
void TIM14_Init(void);
void TIM14_IT_ENABLE(void);


#endif/*__TIMER_H__*/
