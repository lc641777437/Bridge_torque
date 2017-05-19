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

void pc_message_proc(USART_TypeDef *usart, u8 *buf)
{
    int values;
    char tmp[32];
    char *pBuf = NULL;

    pBuf = strstr((char *)buf, "SampleRate:");
    if(pBuf)
    {
        sscanf((const char*)pBuf, "%11s%d", tmp, &values);
        switch(values)
        {
            case 200:
            case 100:
            case 50:
            case 20:
            case 10:
            case 5:
            case 2:
            case 1:
                Write_Frequent(values);
                set_Frequent(values);
                USART_Send_Bytes_Directly(usart, 0x10, NULL, 0);
                break;
            default:
                USART_Send_Bytes_Directly(usart, 0x20, NULL, 0);
                break;
        }
    }

    pBuf = strstr((char *)buf, "SetDeviceID:");
    if(pBuf)
    {
        USART_Send_Bytes_Directly(usart, 0x11, NULL, 0);
        sscanf((const char*)pBuf,"%12s%d", tmp, &values);
        Write_DeviceID(values);
    }

    pBuf = strstr((char *)buf, "SetCtrlState:");
    if(pBuf)
    {
        USART_Send_Bytes_Directly(usart, 0x12, NULL, 0);
        sscanf((const char*)pBuf, "%13s%x", tmp, &values);
        Write_CtrlState(values);
        set_CtrlState(values);
    }

    pBuf = strstr((char *)buf, "SetRemoteIP:");
    if(pBuf)
    {
        sscanf((const char*)pBuf,"%12s%d", tmp, &values);
        if(values >= 0 && values <= 255)
        {
            Write_IPAddress(values);
            reset_DeviceState(DEVICE_ETH);
            USART_Send_Bytes_Directly(usart, 0x13, NULL, 0);
        }
        else
        {
            USART_Send_Bytes_Directly(usart, 0X23, NULL, 0);
        }
    }

    pBuf = strstr((char *)buf, "StartToSend");
    if(pBuf)
    {
        set_Send_Flag(SEND_BY_UART1);
    }

    pBuf = strstr((char *)buf, "StopToSend");
    if(pBuf)
    {
        set_Send_Flag(SEND_BY_NULL);
        USART_Send_Bytes_Directly(usart, 0X15, NULL, 0);
    }

    pBuf = strstr((char *)buf, "ResetOption");
    if(pBuf)
    {
        USART_Send_Bytes_Directly(usart, 0x16, NULL, 0);
        Write_Frequent(100);
        set_Frequent(100);
        Write_CtrlState(0xffff);
        set_CtrlState(0xffff);
        Write_DeviceID(1001);
        Write_IPAddress(1);
    }

    pBuf = strstr((char *)buf, "GetSampleRate");
    if(pBuf)
    {
        u8 rate = 0;
        u32 flash = get_FlashState(3);
        rate = flash;
        USART_Send_Bytes_Directly(usart, 0X17, &rate, 1);
    }

    pBuf = strstr((char *)buf, "GetCtrlState");
    if(pBuf)
    {
        u8 state[2] = {0};
        u32 flash = get_FlashState(1);
        state[0] = (u8)(flash>>8);
        state[1] = (u8)flash;
        USART_Send_Bytes_Directly(usart, 0x18, state, 2);
    }

    memset(buf, 0, USART_MAX_RECV_LEN);
    return;
}

