#include "timer.h"


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

void TIM3_Set(u8 sta)  
{  
    if(sta)  
    {  
        TIM3->CNT=0;        
        TIM3->CR1|=1<<0;     
    }
    else TIM3->CR1&=~(1<<0);
}  

void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
    {
        USART1_REC_timeout();
        TIM3_Set(0); 
    }
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  
}


