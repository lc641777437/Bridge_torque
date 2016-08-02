#include "usart.h"
#include "timer.h"
#include "stmflash.h"
#include "ads1258.h"


#define USART_MAX_RECV_LEN 256

static u8 printf_state;
static u8 data_Count;

static u16 USART3_RX_STA = 0;//接收状态标记    
static u16 USART1_RX_STA = 0;//接收状态标记    
static u16 USART2_RX_STA = 0;//接收状态标记

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
        set_Frequent(data);
        LOG_DEBUG("Set sample rate OK\r\n");
    }
    if(strstr((char *)USART1_RX_BUF, "SetDeviceID:"))
    {
        sscanf((const char*)&USART1_RX_BUF,"%12s%d",command,&data);
        Set_DeviceID(data);
        LOG_DEBUG("Set device ID OK\r\n");
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
        if( data_Count>51 )
        {
            data_Count=0;
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
        else
        {
            Send_AD_RawData(data_Count);
            data_Count++;
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
    send_USART2("%s",USART2_RX_BUF);
    USART2_RX_STA = 0;
    memset(USART2_RX_BUF,'\0',strlen((const char*)USART2_RX_BUF));
    return;
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

    usart3.USART_BaudRate = 115200;
    usart3.USART_WordLength = USART_WordLength_8b;
    usart3.USART_StopBits = USART_StopBits_1;
    usart3.USART_Parity = USART_Parity_No;
    usart3.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    usart3.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3,&usart3);

    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
    USART_Cmd(USART3,ENABLE);

    nvic.NVIC_IRQChannel = USART3_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 3;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}

void USART3_proc(void)
{
    send_USART3("%s",USART3_RX_BUF);
    
    USART3_RX_STA = 0;
    memset(USART3_RX_BUF,'\0',strlen((const char*)USART3_RX_BUF));
    return;
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
}

void UART_Init(void)
{
    USART1_Configuration();
    USART2_Configuration();
	USART3_Configuration();
}
