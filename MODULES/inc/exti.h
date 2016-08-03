#ifndef __EXTI_H
#define __EXTI_H
#include "stm32f4xx.h"
   													  
void EXTI_Configuration(void);
void set_Save_Flag(int i);
int get_Save_Flag(void);

#endif


