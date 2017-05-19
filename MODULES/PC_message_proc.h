/*
 * PC_message_proc.c
 *
 *  Created on: 2017/05/19
 *      Author: lc
 */
#ifndef __PC_MESSAGE_PROC_H__
#define __PC_MESSAGE_PROC_H__
#include "stm32f4xx.h"
typedef enum{
    CMD_SET_SEQUENCE = 0X10,
    CMD_SET_ID       = 0X11,
    CMD_SET_CTRL     = 0X12,
    CMD_SET_ADDR     = 0X13,
    CMD_START_SAMPLE = 0X14,
    CMD_STOP_SAMPLE  = 0X15,
    CMD_FACTORY      = 0X16,
    CMD_GET_SEQUENCE = 0X17,
    CMD_GET_CTRL     = 0X18,

} CMD_TYPE;
void pc_message_proc(USART_TypeDef *usart, u8 *buf);

#endif/*__PC_MESSAGE_PROC_H__*/
