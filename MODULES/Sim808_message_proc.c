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
    if(buf[0] == 0XA6 && buf[1] == 0XA6)
    {
        switch(buf[2])
        {
            case SIM808_SET_PARAM:
                break;
            case SIM808_GET_PARAM:
                break;
            case SIM808_START_DYNAMIC:
                break;
            case SIM808_STOP_DYNAMIC:
                break;
            case SIM808_SET_FACTORY:
                break;
        }
    }
    memset(buf, 0, USART_MAX_RECV_LEN);
    return;
}

