/*
 * usart.h
 *
 *  Created on: 2017/05/18
 *      Author: lc
 */
#ifndef __USART_H__
#define __USART_H__

#include <stdio.h>

#include "stm32f4xx.h"

void UART_Init(void);
void USART2_RECV_Timeout(void);
void USART3_RECV_Timeout(void);

void USART_Send_Bytes_Directly(USART_TypeDef *uart, u8 cmd, u8 *data, int length);

void USART1_Send_Bytes(u8 *data, int length);
void USART2_Send_Bytes(u8 *data, int length);
void USART3_Send_Bytes(u8 *data, int length);


void select_USART(u8 channel);
#define send_USART1(fmt, ...) select_USART(1);\
									printf (fmt, ##__VA_ARGS__)
#define send_USART2(fmt, ...) select_USART(2);\
									printf (fmt, ##__VA_ARGS__)
#define send_USART3(fmt, ...) select_USART(3);\
									printf (fmt, ##__VA_ARGS__)

#define USART_MAX_RECV_LEN (512)

#if 0
//#define LOG_DEBUG(fmt, ...) send_USART1(fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif


#endif/*__USART_H__*/
