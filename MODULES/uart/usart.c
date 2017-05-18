#include "usart.h"
#include "timer.h"
#include "stmflash.h"
#include "ads1258.h"
#include "stmflash.h"
#include "initstate.h"
#include "exti.h"
#include "rtc.h"
/*
    USART1 232
    USART2 SIM808
    USART3 433
*/
#define USART_MAX_RECV_LEN 512

static u8 printf_channel;

/*receive state flag*/
static u16 USART1_RX_STA = 0;//接收状态标记
static u16 USART2_RX_STA = 0;//接收状态标记
static u16 USART3_RX_STA = 0;//接收状态标记

/*receive buffer*/
static u8 USART1_RX_BUF[USART_MAX_RECV_LEN];
static u8 USART2_RX_BUF[USART_MAX_RECV_LEN];
static u8 USART3_RX_BUF[USART_MAX_RECV_LEN];

/*send buffer*/
static u8 USART1_TX_BUF[USART_MAX_RECV_LEN];
static u8 USART2_TX_BUF[USART_MAX_RECV_LEN];
static u8 USART3_TX_BUF[USART_MAX_RECV_LEN];

/*input send position*/
static u32 USART1_TX_ptr_in  = 0;
static u32 USART2_TX_ptr_in  = 0;
static u32 USART3_TX_ptr_in  = 0;

/*output send position*/
static u32 USART1_TX_ptr_out = 0;
static u32 USART2_TX_ptr_out = 0;
static u32 USART3_TX_ptr_out = 0;

/*remain buffer length*/
static u32 USART1_TX_length  = 0;
static u32 USART2_TX_length  = 0;
static u32 USART3_TX_length  = 0;

#pragma import(__use_no_semihosting)
struct __FILE
{
	int handle;
};
FILE __stdout;
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    switch(printf_channel)
    {
        case 1:
            while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
            USART_SendData(USART1, (uint8_t)ch);
            break;

        case 2:
            while (USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
            USART_SendData(USART2, (uint8_t)ch);
            break;

        case 3:
            while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
            USART_SendData(USART3, (uint8_t)ch);

            break;
    }
	return ch;
}

void select_USART(u8 channel)
{
	printf_channel = channel;
}

void USART1_Configuration(void)
{
    USART_InitTypeDef usart1;
    GPIO_InitTypeDef  gpio;
    NVIC_InitTypeDef  nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

    gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &gpio);

    usart1.USART_BaudRate = 115200;
    usart1.USART_WordLength = USART_WordLength_8b;
    usart1.USART_StopBits = USART_StopBits_1;
    usart1.USART_Parity = USART_Parity_No;
    usart1.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    usart1.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &usart1);

    USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);

    nvic.NVIC_IRQChannel = USART1_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 3;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}

void USART1_proc()
{
    int data;
    char command[32];

    if(strstr((char *)USART1_RX_BUF, "SampleRate:"))
    {
        sscanf((const char*)&USART1_RX_BUF,"%11s%d",command,&data);
        switch(data)
        {
            case 200:
            case 100:
            case 50:
            case 20:
            case 10:
            case 5:
            case 2:
            case 1:
                Write_Frequent(data);
                set_Frequent(data);
                USART1_Send_Bytes_Directly(0x10, NULL, 0);
                break;
            default:
                USART1_Send_Bytes_Directly(0x20, NULL, 0);
                break;
        }
    }
    if(strstr((char *)USART1_RX_BUF, "SetDeviceID:"))
    {
        USART1_Send_Bytes_Directly(0x11, NULL, 0);
        sscanf((const char*)&USART1_RX_BUF,"%12s%d",command,&data);
        Write_DeviceID(data);
    }
    if(strstr((char *)USART1_RX_BUF, "SetCtrlState:"))
    {
        USART1_Send_Bytes_Directly(0x12, NULL, 0);
        sscanf((const char*)&USART1_RX_BUF,"%13s%x",command,&data);
        Write_CtrlState(data);
        set_CtrlState(data);
    }
    if(strstr((char *)USART1_RX_BUF, "SetRemoteIP:"))
    {
        sscanf((const char*)&USART1_RX_BUF,"%12s%d",command,&data);
        if(data >= 0&&data <=255)
        {
            Write_IPAddress(data);
            reset_DeviceState(DEVICE_ETH);
            USART1_Send_Bytes_Directly(0x13, NULL, 0);
        }
        else
        {
            USART1_Send_Bytes_Directly(0X23, NULL, 0);
        }
    }
    if(strstr((char *)USART1_RX_BUF, "StartToSend"))
    {
        set_Send_Flag(SEND_BY_UART1);
    }
    if(strstr((char *)USART1_RX_BUF, "StopToSend"))
    {
        set_Send_Flag(SEND_BY_NULL);
        USART1_Send_Bytes_Directly(0X15, NULL, 0);
    }
    if(strstr((char *)USART1_RX_BUF, "ResetOption"))
    {
        USART1_Send_Bytes_Directly(0x16, NULL, 0);
        Write_Frequent(100);
        set_Frequent(100);
        Write_CtrlState(0xffff);
        set_CtrlState(0xffff);
        Write_DeviceID(1001);
        Write_IPAddress(1);
    }
    if(strstr((char *)USART1_RX_BUF, "GetSampleRate"))
    {
        u8 rate = 0;
        u32 flash = get_FlashState(3);
        rate = flash;
        USART1_Send_Bytes_Directly(0X17, &rate, 1);
    }
    if(strstr((char *)USART1_RX_BUF, "GetCtrlState"))
    {
        u8 state[2] = {0};
        u32 flash = get_FlashState(1);
        state[0] = (u8)(flash>>8);
        state[1] = (u8)flash;
        USART1_Send_Bytes_Directly(0x18, state, 2);
    }
    memset(USART1_RX_BUF, 0, USART_MAX_RECV_LEN);
    return;
}


void USART1_IRQHandler(void)
{
    u8 res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);

		res = USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据

        if(USART1_RX_STA & 0x4000)      //之前接收到了0x0d
        {
            USART1_RX_STA = 0;
            if(res == 0x0a)             //接收完成了
            {
                USART1_proc();
            }
        }
        else                            //之前还没收到0X0d
        {
            if(res == 0x0d)
            {
                USART1_RX_STA |= 0x4000;
            }
            else
            {
                USART1_RX_BUF[USART1_RX_STA++ & 0X3FFF] = res;
                if(USART1_RX_STA > (USART_MAX_RECV_LEN - 1))
                {
                    USART1_RX_STA = 0;  //接收数据错误,重新开始接收
                }
            }
        }
    }

    if( USART_GetITStatus(USART1, USART_IT_TXE) == SET )
    {
        if(USART1_TX_length > 0)
        {
            USART_SendData(USART1, USART1_TX_BUF[USART1_TX_ptr_out]);
            USART1_TX_ptr_out++;
            USART1_TX_length--;

            if(USART1_TX_length == 0)
            {
                USART_ITConfig(USART1, USART_IT_TXE, DISABLE);//close the Tx interrupt
            }

            if(USART1_TX_ptr_out == USART_MAX_RECV_LEN)// To avoid buffer overflow
            {
                USART1_TX_ptr_out = 0;
            }
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);//close the Tx interrupt
        }
    }
}

void USART1_Send_Bytes_Directly(u8 cmd, u8 *data, int length)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART_SendData(USART1, 0XA6);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART_SendData(USART1, 0XA6);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART_SendData(USART1, (cmd));
    for(int i = 0; i < length; i++)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        USART_SendData(USART1, *(data + i));
    }
}

static void USART1_Send_Byte(u8 data)
{
	while(USART1_TX_length > USART_MAX_RECV_LEN - 2)//avoid overflow
	{
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}

	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	USART1_TX_BUF[USART1_TX_ptr_in] = data;
	USART1_TX_ptr_in++;
	USART1_TX_length++;

	if(USART1_TX_ptr_in == USART_MAX_RECV_LEN)// avoid overflow
	{
		USART1_TX_ptr_in = 0;
	}

	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);//open the Tx interrupt
}

void USART1_Send_Bytes(u8 *data,int length)
{
	for(int i = 0; i < length; i++)
	{
        USART1_Send_Byte(*(data + i));
	}
}


void USART3_Configuration(void)
{
    USART_InitTypeDef usart3;
    GPIO_InitTypeDef  gpio;
    NVIC_InitTypeDef  nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);

    gpio.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &gpio);

    usart3.USART_BaudRate = 38400;
    usart3.USART_WordLength = USART_WordLength_8b;
    usart3.USART_StopBits = USART_StopBits_1;
    usart3.USART_Parity = USART_Parity_No;
    usart3.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    usart3.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &usart3);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE);

    nvic.NVIC_IRQChannel = USART3_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 2;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}

void USART3_proc(void)
{
    char command[20];
    int data;

    if(strstr((char *)USART3_RX_BUF, "SampleRate:"))
    {
        sscanf((const char*)&USART3_RX_BUF, "%11s%d", command, &data);
        switch(data)
        {
            case 200:
            case 100:
            case 50:
            case 20:
            case 10:
            case 5:
            case 2:
            case 1:
                Write_Frequent(data);
                set_Frequent(data);
                USART3_Send_Bytes_Directly(0x10, NULL, 0);
                break;
            default:
                USART3_Send_Bytes_Directly(0x20, NULL, 0);
                break;
        }
    }
    if(strstr((char *)USART3_RX_BUF, "SetDeviceID:"))
    {
        USART3_Send_Bytes_Directly(0x11, NULL, 0);
        sscanf((const char*)&USART3_RX_BUF,"%12s%d", command,&data);
        Write_DeviceID(data);
    }
    if(strstr((char *)USART3_RX_BUF, "SetCtrlState:"))
    {
        USART3_Send_Bytes_Directly(0x12, NULL, 0);
        sscanf((const char*)&USART3_RX_BUF,"%13s%x", command,&data);
        Write_CtrlState(data);
        set_CtrlState(data);
    }
    if(strstr((char *)USART3_RX_BUF, "SetRemoteIP:"))
    {
        sscanf((const char*)&USART3_RX_BUF,"%12s%d", command,&data);
        if(data >= 0&&data <=255)
        {
            Write_IPAddress(data);
            reset_DeviceState(DEVICE_ETH);
            USART3_Send_Bytes_Directly(0x13, NULL, 0);
        }
        else
        {
            USART3_Send_Bytes_Directly(0x23, NULL, 0);
        }
    }
    if(strstr((char *)USART3_RX_BUF, "StartToSend"))
    {
        set_Send_Flag(SEND_BY_UART3);
    }
    if(strstr((char *)USART3_RX_BUF, "StopToSend"))
    {
        USART3_Send_Bytes_Directly(0x15, NULL, 0);
        set_Send_Flag(SEND_BY_NULL);
    }
    if(strstr((char *)USART3_RX_BUF, "ResetOption"))
    {
        USART3_Send_Bytes_Directly(0x16, NULL, 0);
        Write_Frequent(100);
        set_Frequent(100);
        Write_CtrlState(0xffff);
        set_CtrlState(0xffff);
        Write_DeviceID(1001);
        Write_IPAddress(1);
    }
    if(strstr((char *)USART3_RX_BUF, "GetSampleRate"))
    {
        u32 flash = get_FlashState(3);
        u8 rate = flash;
        USART3_Send_Bytes_Directly(0x17, &rate, 1);
    }
    if(strstr((char *)USART3_RX_BUF, "GetCtrlState"))
    {
        u8 state[2] = {0};
        u32 flash = get_FlashState(1);
        state[0] = (u8)(flash>>8);
        state[1] = (u8)flash;
        USART3_Send_Bytes_Directly(0x18, state, 2);
    }
    memset(USART3_RX_BUF, 0, USART_MAX_RECV_LEN);
    return;
}

void USART3_Send_Bytes_Directly(u8 cmd, u8 *data, int length)
{
    while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    USART_SendData(USART3, 0XA6);
    while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    USART_SendData(USART3, 0XA6);
    while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    USART_SendData(USART3, cmd);
    for(int i = 0; i < length; i++)
    {
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
        USART_SendData(USART3, *(data + i));
    }
}

static void USART3_Send_Byte(u8 data)
{
	while(USART3_TX_length > USART_MAX_RECV_LEN - 2)//avoid overflow
	{
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	}

	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	USART3_TX_BUF[USART3_TX_ptr_in] = data;
	USART3_TX_ptr_in++;
	USART3_TX_length++;

	if(USART3_TX_ptr_in == USART_MAX_RECV_LEN)// avoid overflow
	{
		USART3_TX_ptr_in = 0;
	}

	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);//open the Tx interrupt
}

void USART3_Send_Bytes(u8 *data,int length)
{
	for(int i = 0; i < length; i++)
	{
        USART3_Send_Byte(*(data + i));
	}
}

void USART3_RECV_Timeout(void)// uart3 timeout
{
    USART3_proc();
}

void USART3_IRQHandler(void)
{
    u8 res;

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);

		res = USART_ReceiveData(USART3);//(USART3->DR);	//读取接收到的数据

        if(USART3_RX_STA & 0x4000)      //之前接收到了0x0d
        {
            USART3_RX_STA = 0;
            if(res == 0x0a)             //接收完成了
            {
                TIM3_set(0);
                USART3_proc();
            }
        }
        else                            //之前还没收到0X0d
        {
            if(res == 0x0d)
            {
                USART3_RX_STA |= 0x4000;
            }
            else
            {
                TIM3_set(1);
                USART3_RX_BUF[USART3_RX_STA++ & 0X3FFF] = res;
                if(USART3_RX_STA > (USART_MAX_RECV_LEN - 1))
                {
                    USART3_RX_STA = 0;  //接收数据错误,重新开始接收
                }
            }
        }
    }
    else if( USART_GetITStatus(USART3, USART_IT_TXE) == SET )
    {
        if(USART3_TX_length > 0)
        {
            USART_SendData(USART3, USART3_TX_BUF[USART3_TX_ptr_out]);
            USART3_TX_ptr_out++;
            USART3_TX_length--;

            if(USART3_TX_length == 0)
            {
                USART_ITConfig(USART3, USART_IT_TXE, DISABLE);//close the Tx interrupt
            }

            if(USART3_TX_ptr_out == USART_MAX_RECV_LEN)// To avoid buffer overflow
            {
                USART3_TX_ptr_out = 0;
            }
        }
        else
        {
            USART_ITConfig(USART3, USART_IT_TXE, DISABLE);//close the Tx interrupt
        }
    }
}


void USART2_Configuration(void)
{
    USART_InitTypeDef usart2;
    GPIO_InitTypeDef  gpio;
    NVIC_InitTypeDef  nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2);

    gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD,&gpio);

    usart2.USART_BaudRate = 115200;
    usart2.USART_WordLength = USART_WordLength_8b;
    usart2.USART_StopBits = USART_StopBits_1;
    usart2.USART_Parity = USART_Parity_No;
    usart2.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    usart2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2,&usart2);

    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
    USART_Cmd(USART2,ENABLE);

    nvic.NVIC_IRQChannel = USART2_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 3;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}
void USART2_proc()
{
    int year,month,date,hour,minute,second;
    if(USART2_RX_BUF[0] == '2'&&USART2_RX_BUF[1] == '0')
    {
        sscanf((const char*)&USART2_RX_BUF,"%*2d%2d%2d%2d%2d%2d%2d",
                     &year, &month, &date, &hour, &minute, &second);
        RTC_Set_Time((u8)hour, (u8)minute, (u8)second,RTC_H12_AM);//设置时间
        RTC_Set_Date((u8)year, (u8)month, (u8)date,1);//设置日期
    }
    USART2_RX_STA = 0;
    memset(USART2_RX_BUF, 0, USART_MAX_RECV_LEN);
}

void USART2_Send_Cmd(u8 cmd)
{
    USART2_Send_Byte(0xA6);
    USART2_Send_Byte(0xA6);
    USART2_Send_Byte(cmd);
}

void USART2_Send_Byte(u8 data)
{
	while(USART2_TX_length >= USART_MAX_RECV_LEN - 2)//avoid overflow
	{
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	}

	//USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	USART2_TX_BUF[USART2_TX_ptr_in] = data;
	USART2_TX_ptr_in++;
	USART2_TX_length++;

	if(USART2_TX_ptr_in == USART_MAX_RECV_LEN)// avoid overflow
	{
		USART2_TX_ptr_in = 0;
	}

	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);//open the Tx interrupt
}

void USART2_Send_Bytes(u8 *data,int length)
{
	for(int i = 0; i < length; i++)
	{
        USART2_Send_Byte(*(data + i));
	}
}

void USART2_RECV_Timeout(void)// uart2 timeout
{
     USART2_proc();
}

void USART2_IRQHandler(void)
{
    u8 res;

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);

		res = USART_ReceiveData(USART2);//(USART2->DR);	//读取接收到的数据

        if(USART2_RX_STA & 0x4000)//之前接收到了0x0d
        {
            USART2_RX_STA = 0;
            if(res == 0x0a)//接收完成了
            {
				TIM4_set(0);
                USART2_proc();
            }
        }
        else//之前还没收到0X0d
        {
            if(res == 0x0d)
                USART2_RX_STA |= 0x4000;
            else
            {
				TIM4_set(1);
                USART2_RX_BUF[USART2_RX_STA++ & 0X3FFF] = res;
                if(USART2_RX_STA > (USART_MAX_RECV_LEN - 1))
                {
                    USART2_RX_STA = 0;//接收数据错误,重新开始接收
                }
            }
        }
    }

    if( USART_GetITStatus(USART2, USART_IT_TXE) == SET )
    {
        if(USART2_TX_length > 0)
        {
            USART_SendData(USART2, USART2_TX_BUF[USART2_TX_ptr_out]);
            USART2_TX_ptr_out++;
            USART2_TX_length--;

            if(USART2_TX_length == 0)
            {
                USART_ITConfig(USART2, USART_IT_TXE, DISABLE);//close the Tx interrupt
            }

            if(USART2_TX_ptr_out == USART_MAX_RECV_LEN)// To avoid buffer overflow
            {
                USART2_TX_ptr_out = 0;
            }
        }
        else
        {
            USART_ITConfig(USART2, USART_IT_TXE, DISABLE);//close the Tx interrupt
        }
    }
}


void UART_Init(void)
{
    USART1_Configuration();
    USART2_Configuration();
	USART3_Configuration();
}
