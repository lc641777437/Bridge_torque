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
    CMD_SET_PARAM    = 0X01,
    CMD_GET_PARAM    = 0X02,
    CMD_SEND_START   = 0X03,
    CMD_SEND_STOP    = 0X04,
    CMD_SET_FACTORY  = 0X05,

    CMD_SET_ERROR    = 0X99
} CMD_TYPE;

typedef struct{
    u8 devidH;
    u8 devidL;
    u8 ctrlH;
    u8 ctrlL;
    u8 fre;
    u8 sendTimeServer;
    u8 fre_dynamic;
}MSG_PARAMS;


void pc_message_proc(USART_TypeDef *usart, u8 *buf);

#endif/*__PC_MESSAGE_PROC_H__*/
