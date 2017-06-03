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

static void sim808_set_param(USART_TypeDef *usart, u8 *buf)
{
    u8 res= 0;
    u32 devid = 0, ctrl = 0, fre = 0, sendTimeServer = 0, fre_dynamic = 0;
    MSG_PARAMS *req = (MSG_PARAMS *)buf;

    devid += req->devidH;
    devid = devid << 8;
    devid += req->devidL;

    ctrl += req->ctrlH;
    ctrl = ctrl << 8;
    ctrl += req->ctrlL;

    fre = req->fre;
    if(fre!=200&&fre!=100&&fre!=50&&fre!=20&&fre!=10&&fre!=5&&fre!=2&&fre!=1)res=1;

    sendTimeServer = req->sendTimeServer;
    if(sendTimeServer > 30)res = 1;

    fre_dynamic = req->fre_dynamic;
    if(fre_dynamic!=50&&fre_dynamic!=30&&fre_dynamic!=20&&fre_dynamic!=10&&fre_dynamic!=5&&fre_dynamic!=2&&fre_dynamic!=1)res=1;

    if(res == 0)
    {
        USART2_Send_Bytes_Directly(SIM808_SET_PARAM, NULL, 0);
        set_DevID(devid);
        set_CtrlState(ctrl);
        set_Frequent(fre);
        set_SendTimeServer(sendTimeServer);
        set_SendTimeDynamic(fre_dynamic);
        flash_setValues(devid, ctrl, fre, sendTimeServer, fre_dynamic);
    }
    else
    {
        USART2_Send_Bytes_Directly(SIM808_SET_ERROR, NULL, 0);
    }
}

static void sim808_get_param(USART_TypeDef *usart, u8 *buf)
{
    u32 flash = 0;
    MSG_PARAMS msg;

    flash = flash_getValue(FLASH_DEVID);
    msg.devidH = (u8)(flash>>8);
    msg.devidL = (u8)flash;

    flash = flash_getValue(FLASH_CTRL);
    msg.ctrlH = (u8)(flash>>8);
    msg.ctrlL = (u8)flash;

    flash = flash_getValue(FLASH_FREQUENCE);
    msg.fre = (u8)flash;
    flash = flash_getValue(FLASH_SENDTIME_SERVER);
    msg.sendTimeServer = (u8)flash;
    flash = flash_getValue(FLASH_SENDTIME_DYNAMIC);
    msg.fre_dynamic = (u8)flash;

    USART2_Send_Bytes_Directly(SIM808_GET_PARAM, (u8 *)&msg, sizeof(MSG_PARAMS));
}

static void sim808_start_dynamic(USART_TypeDef *usart, u8 *buf)
{
    set_isDynamic(1);
    USART2_Send_Bytes_Directly(SIM808_START_DYNAMIC, NULL, 0);
}

static void sim808_stop_dynamic(USART_TypeDef *usart, u8 *buf)
{
    set_isDynamic(0);
    USART2_Send_Bytes_Directly(SIM808_STOP_DYNAMIC, NULL, 0);
}

static void sim808_set_factory(USART_TypeDef *usart, u8 *buf)
{
    USART2_Send_Bytes_Directly(SIM808_SET_FACTORY, NULL, 0);
    set_DevID(1001);
    set_CtrlState(0xFFFF);
    set_Frequent(200);
    set_SendTimeServer(1);
    set_SendTimeDynamic(1);
    flash_setValues(1001, 0xFFFF, 200, 1, 200);
}

void sim808_message_proc(USART_TypeDef *usart, u8 *buf)
{
    if(buf[0] == 0XA6 && buf[1] == 0XA6)
    {
        switch(buf[2])
        {
            case SIM808_SET_PARAM://response to server
                sim808_set_param(usart, buf + 3);
                break;

            case SIM808_GET_PARAM://response to server
                sim808_get_param(usart, buf + 3);
                break;

            case SIM808_START_DYNAMIC://response to server
                sim808_start_dynamic(usart, buf + 3);
                break;

            case SIM808_STOP_DYNAMIC://response to server
                sim808_stop_dynamic(usart, buf + 3);
                break;

            case SIM808_SET_FACTORY://response to server
                sim808_set_factory(usart, buf + 3);
                break;

            case SIM808_SET_TIMESTAMP://response to server
                sim808_set_timestamp(usart, buf + 3);
                break;

            case SIM808_SET_SERVER://response to pc
                sim808_set_server(usart, buf + 3);
                break;

            case SIM808_SET_ERROR://response to pc
                sim808_set_error(usart, buf + 3);
                break;
        }
    }
    memset(buf, 0, USART_MAX_RECV_LEN);
    return;
}

