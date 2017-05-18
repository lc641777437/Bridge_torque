#ifndef __EXTI_H
#define __EXTI_H
#include "stm32f4xx.h"

typedef enum{
    SEND_BY_NULL = 0,
    SEND_BY_UART1= 1,
    SEND_BY_UART3= 2
} SEND_BY_WHO;

typedef enum{
    SAVE_BY_NULL = 0,
    SAVE_BY_SD   = 1,
    SAVE_BY_USB  = 2
} SAVE_BY_WHO;


void EXTI_Configuration(void);

void set_Save_Flag(int flag);
int get_Save_Flag(void);

void set_Send_Flag(SEND_BY_WHO who);
SEND_BY_WHO get_Send_Flag(void);

extern u8 sample_count;

#endif

