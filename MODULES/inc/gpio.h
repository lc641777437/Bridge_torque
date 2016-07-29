#ifndef __GPIO_H
#define __GPIO_H
#include "stm32f4xx.h"
   							
#define STATE_1 PEin(5)
#define STATE_2 PEin(4)
#define STATE_3 PEin(3)
#define STATE_4 PEin(2)
#define STATE_5 PAin(11)
#define STATE_6 PAin(10)
#define STATE_7 PAin(9)
#define STATE_8 PAin(8)
#define STATE_9 PEin(15)
#define STATE_10 PEin(14)
#define STATE_11 PEin(13)
#define STATE_12 PEin(12)
#define STATE_13 PEin(11)
#define STATE_14 PEin(10)
#define STATE_15 PEin(9)
#define STATE_16 PEin(8)

void GPIO_init(void);
		 
#endif
