#include "timer.h"
#include "usart.h"

extern u32 lwip_localtime;

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
        USART1_RECV_Timeout();
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
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 
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


