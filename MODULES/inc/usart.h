#ifndef __USART_H
#define __USART_H

#include <stdio.h>
#include "stm32f4xx.h"

void select_USART(u8 channel);

void USART1_RECV_Timeout(void);
void USART3_RECV_Timeout(void);

void UART_Init(void);

#define send_USART1(fmt, ...) select_USART(1);\
									printf (fmt, ##__VA_ARGS__)
#define send_USART2(fmt, ...) select_USART(2);\
									printf (fmt, ##__VA_ARGS__)
#define send_USART3(fmt, ...) select_USART(3);\
									printf (fmt, ##__VA_ARGS__)
                                    
#define LOG_DEBUG(fmt, ...) send_USART2("[%s][%d]"fmt,__FILE__,__LINE__,##__VA_ARGS__)

                                    
#endif



