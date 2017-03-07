#include "usart.h"
#include "timer.h"
#include "stmflash.h"
#include "ads1258.h"
#include "stmflash.h"
#include "initstate.h"
#include "exti.h"
#include "rtc.h"


#define USART_MAX_RECV_LEN 256

static u8 printf_state;
static u8 data_Count_1;
static u8 data_Count_2;
static u8 data_Count_3;

static u16 USART1_RX_STA = 0;//接收状态标记    
static u16 USART2_RX_STA = 0;//接收状态标记
static u16 USART3_RX_STA = 0;//接收状态标记    

static u8 USART1_RX_BUF[USART_MAX_RECV_LEN];    
static u8 USART2_RX_BUF[USART_MAX_RECV_LEN];
static u8 USART3_RX_BUF[USART_MAX_RECV_LEN];    


#pragma import(__use_no_semihosting)             
               
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       

//重定义fputc函数
int fputc(int ch, FILE *f)
{
    switch(printf_state)
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
	printf_state = channel;
}

void USART1_Configuration(void)
{
    USART_InitTypeDef usart1;
    GPIO_InitTypeDef  gpio;
    NVIC_InitTypeDef  nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1); 

    gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB,&gpio);

    usart1.USART_BaudRate = 115200;
    usart1.USART_WordLength = USART_WordLength_8b;
    usart1.USART_StopBits = USART_StopBits_1;
    usart1.USART_Parity = USART_Parity_No;
    usart1.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    usart1.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1,&usart1);

    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
    USART_Cmd(USART1,ENABLE);

    nvic.NVIC_IRQChannel = USART1_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 3;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}

void USART1_proc()
{
    char command[20];
    int data;
	
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
                sendBackMessage_1(0x10);
                break;
            default:
                sendBackMessage_1(0x20);
                break;
        }
    }
    if(strstr((char *)USART1_RX_BUF, "SetDeviceID:"))
    {
        sendBackMessage_1(0x11);
        sscanf((const char*)&USART1_RX_BUF,"%12s%d",command,&data);
        Write_DeviceID(data);
    }
    if(strstr((char *)USART1_RX_BUF, "SetCtrlState:"))
    {
        sendBackMessage_1(0x12);
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
            reset_InitState(ETHSTATE);
            sendBackMessage_1(0x13);
        }
        else
        {
            sendBackMessage_1(0x23);
        }
    }
    if(strstr((char *)USART1_RX_BUF, "StartToSend"))
    {
        set_Send_Flag(1);
    }
    if(strstr((char *)USART1_RX_BUF, "StopToSend"))
    {
        sendBackMessage_1(0x15);
        set_Send_Flag(0);
    }
    if(strstr((char *)USART1_RX_BUF, "ResetOption"))
    {
        sendBackMessage_1(0x16);
        Write_Frequent(100);
        set_Frequent(100);
        Write_CtrlState(0xffff);
        set_CtrlState(0xffff);
        Write_DeviceID(1001);
        Write_IPAddress(1);
    }
    if(strstr((char *)USART1_RX_BUF, "GetSampleRate"))
    {
        u32 flash;
        flash=get_FlashState(3);
        sendBackMessage_1(0x17);
        while (USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
        USART_SendData(USART1,(u8)flash);
    }
    if(strstr((char *)USART1_RX_BUF, "GetCtrlState"))
    {
        u32 flash;
        flash=get_FlashState(1);
        sendBackMessage_1(0x18);
        while (USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
        USART_SendData(USART1,(u8)(flash>>8));
        while (USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
        USART_SendData(USART1,(u8)flash);
    }
    memset(USART1_RX_BUF,'\0',strlen((const char*)USART1_RX_BUF));
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
                USART1_RX_STA |= 0x4000;
            else
            {
                USART1_RX_BUF[USART1_RX_STA++ & 0X3FFF] = res;
                if(USART1_RX_STA > (USART_MAX_RECV_LEN - 1))
                    USART1_RX_STA = 0;  //接收数据错误,重新开始接收	  
            }		 
        }	  		 
    }
    if( USART_GetITStatus(USART1, USART_IT_TXE) == SET  )
    {
        if( data_Count_1>51 )
        {
            data_Count_1=0;
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
        else
        {
            Send_AD_RawData_1(data_Count_1);
            data_Count_1++;
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
    u8 year,month,date,hour,minute,second;
    if(USART2_RX_BUF[0] == '2'&&USART2_RX_BUF[1] == '0')
    {
        sscanf((const char*)&USART2_RX_BUF,"%*2d%2d%2d%2d%2d%2d%2d",&year,&month,&date,&hour,&minute,&second);
        RTC_Set_Time(hour,minute,second,RTC_H12_AM);	//设置时间
        RTC_Set_Date(year,month,date,1);		//设置日期 
    }
    USART2_RX_STA = 0;
    memset(USART2_RX_BUF,'\0',strlen((const char*)USART2_RX_BUF));
}

void USART2_RECV_Timeout(void)
{
     USART2_proc();
}

void USART2_IRQHandler(void)
{
    u8 res;
 
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);

		res = USART_ReceiveData(USART2);//(USART2->DR);	//读取接收到的数据
		

        if(USART2_RX_STA & 0x4000)      //之前接收到了0x0d
        {
            USART2_RX_STA = 0;
            if(res == 0x0a)             //接收完成了
            {
				TIM4_set(0);
                USART2_proc();
            }
        }
        else                            //之前还没收到0X0d
        {	
            if(res == 0x0d)
                USART2_RX_STA |= 0x4000;
            else
            {
				TIM4_set(1);
                USART2_RX_BUF[USART2_RX_STA++ & 0X3FFF] = res;
                if(USART2_RX_STA > (USART_MAX_RECV_LEN - 1))
                    USART2_RX_STA = 0;  //接收数据错误,重新开始接收	  
            }		 
        }	  		 
    }
    if( USART_GetITStatus(USART2, USART_IT_TXE) == SET  )
    {
        if( data_Count_2>147 )
        {
            data_Count_2=0;
            USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
        }
        else
        {
            Send_AD_RawData_2(data_Count_2);
            data_Count_2++;
        }
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
    GPIO_Init(GPIOB,&gpio);

    usart3.USART_BaudRate = 38400;
    usart3.USART_WordLength = USART_WordLength_8b;
    usart3.USART_StopBits = USART_StopBits_1;
    usart3.USART_Parity = USART_Parity_No;
    usart3.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    usart3.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3,&usart3);

    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
    USART_Cmd(USART3,ENABLE);

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
        sscanf((const char*)&USART3_RX_BUF,"%11s%d",command,&data);
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
                sendBackMessage_3(0x10);
                break;
            default:
                break;
        }

    }
    if(strstr((char *)USART3_RX_BUF, "SetDeviceID:"))
    {
        sendBackMessage_3(0x11);
        sscanf((const char*)&USART3_RX_BUF,"%12s%d",command,&data);
        Write_DeviceID(data);
    }
    if(strstr((char *)USART3_RX_BUF, "SetCtrlState:"))
    {
        sendBackMessage_3(0x12);
        sscanf((const char*)&USART3_RX_BUF,"%13s%x",command,&data);
        Write_CtrlState(data);
        set_CtrlState(data);
    }
    if(strstr((char *)USART3_RX_BUF, "SetRemoteIP:"))
    {
        sscanf((const char*)&USART3_RX_BUF,"%12s%d",command,&data);
        if(data>=0&&data<=255)
        {
            Write_IPAddress(data);
            reset_InitState(ETHSTATE);
            sendBackMessage_3(0x13);
        }
    }
    if(strstr((char *)USART3_RX_BUF, "StartToSend"))
    {
        set_Send_Flag(2);
    }
    if(strstr((char *)USART3_RX_BUF, "StopToSend")!=NULL)
    {
        sendBackMessage_3(0x15);
        set_Send_Flag(0);
    }
    if(strstr((char *)USART3_RX_BUF, "ResetOption"))
    {
        sendBackMessage_3(0x16);
        Write_Frequent(10);
        set_Frequent(10);
        Write_CtrlState(0xffff);
        set_CtrlState(0xffff);
        Write_DeviceID(1001);
        Write_IPAddress(1);
    }
    if(strstr((char *)USART3_RX_BUF, "GetSampleRate"))
    {
        u32 flash;
        flash=get_FlashState(3);
        sendBackMessage_3(0x17);
        while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
        USART_SendData(USART3,(u8)flash);
    }
    if(strstr((char *)USART3_RX_BUF, "GetCtrlState"))
    {
        u32 flash;
        flash=get_FlashState(1);
        sendBackMessage_3(0x18);
        while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
        USART_SendData(USART3,(u8)(flash>>8));
        while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
        USART_SendData(USART3,(u8)flash);
    }
    memset(USART3_RX_BUF,'\0',strlen((const char*)USART3_RX_BUF));
    return;
}

void sendBackMessage_1(u8 data)
{
    while (USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
    USART_SendData(USART1, 0xA6);
    while (USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
    USART_SendData(USART1, 0xA6);
    while (USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
    USART_SendData(USART1, data);
}

void sendBackMessage_3(u8 data)
{
    while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
    USART_SendData(USART3, 0xA6);
    while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
    USART_SendData(USART3, 0xA6);
    while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
    USART_SendData(USART3, data);
}

void USART3_RECV_Timeout(void)
{
     USART3_proc();
}

void USART3_IRQHandler(void)
{
    u8 res;
 
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);

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
                USART3_RX_STA |= 0x4000;
            else
            {
                TIM3_set(1);
                USART3_RX_BUF[USART3_RX_STA++ & 0X3FFF] = res;
                if(USART3_RX_STA > (USART_MAX_RECV_LEN - 1))
                    USART3_RX_STA = 0;  //接收数据错误,重新开始接收	  
            }		 
        }	  		 
    }
    if( USART_GetITStatus(USART3, USART_IT_TXE) == SET  )
    {
        if( data_Count_3>51 )
        {
            data_Count_3=0;
            USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
        }
        else
        {
            Send_AD_RawData_3(data_Count_3);
            data_Count_3++;
        }
    }
}

void UART_Init(void)
{
    USART1_Configuration();
    USART2_Configuration();
	USART3_Configuration();
}
