#ifndef __GPIO_H
#define __GPIO_H
#include "stm32f4xx.h"
   							
#define STATE_0 PFin(2)
#define STATE_1 PFin(1)
#define STATE_2 PFin(0)
#define STATE_3 PEin(6)
#define STATE_4 PEin(5)
#define STATE_5 PEin(4)
#define STATE_6 PEin(3)
#define STATE_7 PEin(2)
#define STATE_8 PFin(3)
#define STATE_9 PFin(4)
#define STATE_10 PFin(5)
#define STATE_11 PFin(6)
#define STATE_12 PFin(7)
#define STATE_13 PFin(8)
#define STATE_14 PFin(9)
#define STATE_15 PFin(10)

#define CTRL_0 PEout(9)
#define CTRL_1 PEout(8)
#define CTRL_2 PEout(7)
#define CTRL_3 PGout(1)
#define CTRL_4 PGout(0)
#define CTRL_5 PFout(15)
#define CTRL_6 PFout(14)
#define CTRL_7 PFout(13)
#define CTRL_8 PGout(8)
#define CTRL_9 PGout(7)
#define CTRL_10 PGout(6)
#define CTRL_11 PGout(5)
#define CTRL_12 PGout(4)
#define CTRL_13 PGout(3)
#define CTRL_14 PGout(2)
#define CTRL_15 PDout(15)


void GPIO_init(void);
		 
#endif
