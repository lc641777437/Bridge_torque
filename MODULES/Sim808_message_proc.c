/*
 * Sim808_message_proc.c
 *
 *  Created on: 2017/05/19
 *      Author: lc
 */
#include <string.h>

#include "exti.h"
#include "usart.h"
#include "timer.h"
#include "ads1258.h"
#include "setting.h"
#include "stmflash.h"
#include "initstate.h"
#include "Sim808_message_proc.h"

void sim808_message_proc(USART_TypeDef *usart, u8 *buf)
{
    memset(buf, 0, USART_MAX_RECV_LEN);
    return;
}

