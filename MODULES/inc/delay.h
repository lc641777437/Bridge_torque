#ifndef __DELAY_H__
#define __DELAY_H__
#include "stm32f4xx.h"

void delay_init(void);
void delay_us(u32 nus);
void delay_xms(u16 nms);
void delay_ms(u16 nms);

#endif
