#ifndef __INITSTATE_H
#define __INITSTATE_H
#include "stm32f4xx.h"

#define SDSTATE 1
#define ETHSTATE 2
#define USBSTATE 3

void add_InitState(char mode);
void reset_InitState(char mode);
char get_InitState(char mode);

		 
#endif


