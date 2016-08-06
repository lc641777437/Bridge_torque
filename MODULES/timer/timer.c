#include "sdio_sdcard.h"
#include "timer.h"
#include "usart.h"
#include "sys.h"
#include "ads1258.h"
#include "initstate.h"
#include "lwip_comm.h"


extern u32 lwip_localtime;
int time_10us=0;
int time_1s=0;
int time_Set=5 * 100;
u8 Sign_Flag=0;//0:master  1:slave 

void TIM_Init(void)
{    
    TIM2_Init();
    TIM3_Init();
    TIM4_Init();
    TIM5_Init();  
    TIM14_Init();    
}
void TIM3_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = 99; 
    TIM_TimeBaseInitStructure.TIM_Prescaler=8399;  
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

void TIM4_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = 99; 
    TIM_TimeBaseInitStructure.TIM_Prescaler=8399;  
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

void TIM5_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = 999; 
    TIM_TimeBaseInitStructure.TIM_Prescaler=839;  
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

void TIM2_Init(void)
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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; 
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM2,ENABLE); 
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{
        if(time_10us<time_Set)
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
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
}

void set_Frequent(int fre)
{
    time_Set=100000/fre;
}

void TIM14_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;  
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
}

void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM14,TIM_IT_Update)==SET)
    {
        if(time_1s<10)
        {
            time_1s++;
        }
        else
        {
            time_1s=0;
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
            if(get_InitState(ETHSTATE)==TCP_OK)
            {
                u16 eth_sta;
                eth_sta=ETH_ReadPHYRegister(0x00,1);
                if((eth_sta&0x00f0)==0x0000)
                {
                    reset_InitState(ETHSTATE);
                    LOG_DEBUG("ETH LOST\r\n");
                }
            }
            else
            {
                LWIP_Init();
            }
        }
    }
    TIM_ClearITPendingBit(TIM14,TIM_IT_Update); 
}

