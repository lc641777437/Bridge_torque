/*
 * PC_message_proc.c
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

#define CMD_STOPSEND_STR "StopToSend"
#define CMD_STARTSEND_STR "StartToSend"

#define CMD_FACTORY_STR "ResetOption"

#define CMD_GETPARAM_STR "GetParam"
#define CMD_SETPARAM_STR "SetParam"
#define CMD_PARAMSTR_LEN (8)

#define CMD_SETSERVER_STR "SetServer"
#define CMD_SETSERVER_LEN (9)

void pc_message_proc(USART_TypeDef *usart, u8 *buf)
{
    char *pBuf = NULL;

    pBuf = strstr((char *)buf, CMD_SETPARAM_STR);
    if(pBuf)
    {
        u8 res= 0;
        u32 devid = 0, ctrl = 0, fre = 0, sendTimeServer = 0, fre_dynamic = 0;
        MSG_PARAMS *req = (MSG_PARAMS *)(pBuf + CMD_PARAMSTR_LEN);

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
            USART_Send_Bytes_Directly(usart, CMD_SET_PARAM, NULL, 0);
            set_DevID(devid);
            set_CtrlState(ctrl);
            set_Frequent(fre);
            set_SendTimeServer(sendTimeServer);
            set_SendTimeDynamic(fre_dynamic);
            flash_setValues(devid, ctrl, fre, sendTimeServer, fre_dynamic);
        }
        else
        {
            USART_Send_Bytes_Directly(usart, CMD_SET_ERROR, NULL, 0);
        }
    }

    pBuf = strstr((char *)buf, CMD_GETPARAM_STR);
    if(pBuf)
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

        USART_Send_Bytes_Directly(usart, CMD_GET_PARAM, (u8 *)&msg, sizeof(MSG_PARAMS));
    }

    pBuf = strstr((char *)buf, CMD_STARTSEND_STR);
    if(pBuf)
    {
        if(usart == USART1)
        {
            set_Send_Flag(SEND_BY_UART1);
        }
        else if(usart == USART3)
        {
            set_Send_Flag(SEND_BY_UART3);
        }
        USART_Send_Bytes_Directly(usart, CMD_SEND_START, NULL, 0);
    }

    pBuf = strstr((char *)buf, CMD_STOPSEND_STR);
    if(pBuf)
    {
        set_Send_Flag(SEND_BY_NULL);
        USART_Send_Bytes_Directly(usart, CMD_SEND_STOP, NULL, 0);
    }

    pBuf = strstr((char *)buf, CMD_FACTORY_STR);
    if(pBuf)
    {
        USART_Send_Bytes_Directly(usart, CMD_SET_FACTORY, NULL, 0);
        set_DevID(1001);
        set_CtrlState(0xFFFF);
        set_Frequent(200);
        set_SendTimeServer(1);
        set_SendTimeDynamic(1);
        flash_setValues(1001, 0xFFFF, 200, 1, 200);
    }


    pBuf = strstr((char *)buf, CMD_SETSERVER_STR);
    if(pBuf)
    {
    }

    memset(buf, 0, USART_MAX_RECV_LEN);
    return;
}

