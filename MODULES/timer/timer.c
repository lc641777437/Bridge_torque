#include "sdio_sdcard.h"
#include "timer.h"
#include "usart.h"
#include "sys.h"
#include "ads1258.h"
#include "initstate.h"
#include "lwip_comm.h"
#include "usbh_usr.h" 
#include "fatfs_api.h"
#include "lcd12864.h"

extern u32 lwip_localtime;
int time_10us = 0;
int time_s = 0;
int sample_time = 5 * 100;
u8 Sign_Flag=0;//0:master  1:slave 
extern USBH_HOST  USB_Host;
extern USB_OTG_CORE_HANDLE  USB_OTG_Core;

void TIM_Init(void)
{    
    TIM2_Init();
    TIM3_Init();
    TIM4_Init();
    TIM5_Init();
    TIM7_Init();
    TIM14_Init();
}
void TIM3_Init(void)//10ms for identify uart3 end
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = 100 - 1; 
    TIM_TimeBaseInitStructure.TIM_Prescaler = 8400 - 1;  
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
    
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); 

    NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM3,ENABLE); 
}

void TIM3_set(u8 sta)  
{
    TIM3->CNT=0;            
    if(sta)  
    {  
        TIM3->CR1|=1<<0;     
    }
    else 
    {
        TIM3->CR1&=~(1<<0);
    }
}  

void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
    {
        USART3_RECV_Timeout();
        TIM3_set(0); 
    }
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  
}

void TIM4_Init(void)//10ms for identify uart2 end
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = 100 - 1; 
    TIM_TimeBaseInitStructure.TIM_Prescaler = 8400 - 1;  
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
    
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); 

    NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM4,ENABLE); 
}

void TIM4_set(u8 sta)  
{
    TIM4->CNT=0;            
    if(sta)  
    {  
        TIM4->CR1|=1<<0;     
    }
    else 
    {
        TIM4->CR1&=~(1<<0);
    }
}  

void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET)
    {
        USART2_RECV_Timeout();
        TIM4_set(0); 
    }
    TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  
}

void TIM5_Init(void)//10ms
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1; 
    TIM_TimeBaseInitStructure.TIM_Prescaler= 840 - 1;  
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);
    
    TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); 

    NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; 
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM5,ENABLE); 
}

void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //溢出中断
	{
		lwip_localtime +=10; //加10
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //清除中断标志位
}

void TIM2_Init(void)//10us
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = 420 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler= 2 - 1;  
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
    
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); 

    NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM2,ENABLE); 
}

void TIM2_IRQHandler(void)//10us
{
    static u8 LCDEN_TIME;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{
        if(time_10us < sample_time-1)
        {
            time_10us++;
        }
        else
        {
            time_10us=0;
            if(Sign_Flag==0)
            {
                Sign_OUT=1;
                START=1;
            }
            else
            {
                Sign_Flag--;
            }
        }
        
        if(isLCDEN())//let LCD_EN high at least 10us
        {
            if(++LCDEN_TIME > 2)
            {
                LCDEN_TIME = 0;
                LCD_Proc_10us();
            }
        }
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
}

void set_Frequent(int fre)
{
    sample_time = 100000/fre;
}

void TIM14_Init(void)//1s: for check the state of SD card and eth
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = 5000 - 1;  
    TIM_TimeBaseInitStructure.TIM_Prescaler= 8400 - 1;  
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM14,&TIM_TimeBaseInitStructure);
    
    TIM_ITConfig(TIM14,TIM_IT_Update,ENABLE); 

    NVIC_InitStructure.NVIC_IRQChannel=TIM8_TRG_COM_TIM14_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM14,ENABLE); 
    TIM_ITConfig(TIM14,TIM_IT_Update,DISABLE); 
}

void TIM14_ENABLE(void)
{
    TIM_ITConfig(TIM14,TIM_IT_Update,ENABLE); 
}

void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM14,TIM_IT_Update)==SET)
    {
        show_Update();
        if(time_s<19)
        {
            time_s++;
        }
        else
        {
            time_s=0;
            if(get_InitState(SDSTATE)==FATFS_OK)
            {
                if(SD_GetState()==SD_CARD_ERROR)
                {
                    reset_InitState(SDSTATE);
                    LOG_DEBUG("SD LOST\r\n");
                }
            }
            else
            {
                SD_Card_Init();
            }
            
            if(get_InitState(ETHSTATE)!= TCP_OK)
            {
                LWIP_Init();
            }
        }
    }
    TIM_ClearITPendingBit(TIM14,TIM_IT_Update); 
}

void TIM7_Init(void)//5ms
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = 5000 - 1; 
    TIM_TimeBaseInitStructure.TIM_Prescaler= 84 - 1;  
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);
    
    TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); 

    NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; 
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM7,DISABLE); 
}

void TIM7_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) //溢出中断
	{
        LCD_Proc_5ms();
	}
	TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  //清除中断标志位
}

void TIM7_Enable(u8 state)
{
    if(state)
    {
        TIM_Cmd(TIM7,ENABLE); 
    }
    else
    {
        TIM_Cmd(TIM7,DISABLE); 
    }
    TIM7->CNT = 0;
}
