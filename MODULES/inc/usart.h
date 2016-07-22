#ifndef __USART_H
#define __USART_H
#include <stdio.h>
#include "stm32f4xx.h"


void select_USART(int channel);
void USART1_Configuration(void);
void USART1_IRQHandler(void);
void USART2_Configuration(void);
void USART2_IRQHandler(void);
void USART3_Configuration(void);
void USART3_IRQHandler(void);

#define send_Gps(fmt, ...) select_USART(1);\
									printf (fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) select_USART(2);\
									printf (fmt, ##__VA_ARGS__)
#define send_Gprs(fmt, ...) select_USART(3);\
									printf (fmt, ##__VA_ARGS__)

                                    
#endif



