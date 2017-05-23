/*
 * Sim_message_proc.h
 *
 *  Created on: 2017/05/19
 *      Author: lc
 */
#ifndef __SIM808_MESSAGE_PROC_H__
#define __SIM808_MESSAGE_PROC_H__
#include "stm32f4xx.h"
typedef enum{
    SIM808_SET_PARAM    = 0X01,
    SIM808_GET_PARAM,
    SIM808_START_DYNAMIC,
    SIM808_STOP_DYNAMIC,
    SIM808_SET_FACTORY,
    SIM808_SET_TIMESTAMP,
    SIM808_SET_SERVER,


    SIM808_SET_ERROR    = 0X99
} CMD_SIM808_TYPE;

void sim808_message_proc(USART_TypeDef *usart, u8 *buf);

#endif/*__SIM808_MESSAGE_PROC_H__*/

