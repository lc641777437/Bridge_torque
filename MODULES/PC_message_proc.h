/*
 * PC_message_proc.c
 *
 *  Created on: 2017/05/19
 *      Author: lc
 */
#ifndef __PC_MESSAGE_PROC_H__
#define __PC_MESSAGE_PROC_H__
#include "stm32f4xx.h"

void pc_message_proc(USART_TypeDef *usart, u8 *buf);

#endif/*__PC_MESSAGE_PROC_H__*/
