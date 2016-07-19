#include "usart.h"


u8 res;
static u8 printf_state;

/*-----USART1_TX-----PB6-----*/
/*-----USART1_RX-----PB7-----*/



#if 1
#pragma import(__use_no_semihosting)             
               
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       

//重定义fputc函数
int fputc(int ch, FILE *f)
{
    if(printf_state==1)
    {
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
        USART_SendData(USART1, (uint8_t)ch);
	}
	else if(printf_state==3)
	{
		while (USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
		USART_SendData(USART3, (uint8_t)ch);
	}
	return ch;
}

int GetKey (void) 
{
	while (!(USART1->SR & USART_FLAG_RXNE));
	return ((int)(USART1->DR & 0x1FF));
}
#endif


void select_USART(int channel)
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
    nvic.NVIC_IRQChannelPreemptionPriority = 3;
    nvic.NVIC_IRQChannelSubPriority = 3;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}


void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		res=USART_ReceiveData(USART1);
        send_Gps("%c",res);
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
    nvic.NVIC_IRQChannelPreemptionPriority = 3;
    nvic.NVIC_IRQChannelSubPriority = 2;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}


void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);
        res=USART_ReceiveData(USART3);
        send_Gprs("%c",res);
    }
}


