#ifndef __USART_H
#define __USART_H

#include <stdio.h>
#include "stm32f4xx.h"

void select_USART(u8 channel);

void USART2_RECV_Timeout(void);
void USART3_RECV_Timeout(void);
void Send_Data(u8 *SendBuf);
void UART_Init(void);
void sendBackMessage_1(u8 data);
void sendBackMessage_3(u8 data);

#define send_USART1(fmt, ...) select_USART(1);\
									printf (fmt, ##__VA_ARGS__)
#define send_USART2(fmt, ...) select_USART(2);\
									printf (fmt, ##__VA_ARGS__)
#define send_USART3(fmt, ...) select_USART(3);\
									printf (fmt, ##__VA_ARGS__)
                                    
//#define LOG_DEBUG(fmt, ...) send_USART1("[%s][%d]"fmt,__FILE__,__LINE__,##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)  

                                    
#endif



