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
#include "PC_message_proc.h"
#include "Sim808_message_proc.h"

static void sim808_set_timestamp(USART_TypeDef *usart, u8 *buf)
{
    u32 timestamp = 0;
    timestamp = buf[0];
    timestamp <<= 8;
    timestamp |= buf[1];
    timestamp <<= 8;
    timestamp |= buf[2];
    timestamp <<= 8;
    timestamp |= buf[3];
    if(timestamp > ZERO_OF_21_CENTURE && timestamp < END_OF_21_CENTURE)
    {
        set_timestamp(timestamp);
        USART_Send_Bytes_Directly(usart, SIM808_SET_TIMESTAMP, NULL, 0);
    }
    else
    {
        USART_Send_Bytes_Directly(usart, SIM808_SET_ERROR, NULL, 0);
    }
}

static void sim808_set_server(USART_TypeDef *usart, u8 *buf)
{
    USART_Send_Bytes_Directly(USART1, CMD_SET_SERVER, NULL, 0);
    USART_Send_Bytes_Directly(USART3, CMD_SET_SERVER, NULL, 0);
}

static void sim808_set_error(USART_TypeDef *usart, u8 *buf)
{
    USART_Send_Bytes_Directly(USART1, CMD_SET_ERROR, NULL, 0);
    USART_Send_Bytes_Directly(USART3, CMD_SET_ERROR, NULL, 0);
}


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
            case SIM808_SET_TIMESTAMP:
                sim808_set_timestamp(usart, buf + 3);
                break;
            case SIM808_SET_SERVER:
                sim808_set_server(usart, buf + 3);
                break;

            case SIM808_SET_ERROR:
                sim808_set_error(usart, buf + 3);
                break;
        }
    }
    memset(buf, 0, USART_MAX_RECV_LEN);
    return;
}

