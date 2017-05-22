/*
 * Sim_message_proc.h
 *
 *  Created on: 2017/05/19
 *      Author: lc
 */
#ifndef __PC_MESSAGE_PROC_H__
#define __PC_MESSAGE_PROC_H__
#include "stm32f4xx.h"
typedef enum{
    CMD_SET_CTRL,
    CMD_GET_CTRL,
    CMD_SET_SEQUENCE,
    CMD_GET_SEQUENCE,
    CMD_START_DYNAMIC,
    CMD_STOP_DYNAMIC,
    CMD_SET_ADDR,
    CMD_FACTORY,

    CMD_SET_ERROR    = 0X99
} CMD_TYPE;

void sim808_message_proc(USART_TypeDef *usart, u8 *buf);

#endif/*__PC_MESSAGE_PROC_H__*/

