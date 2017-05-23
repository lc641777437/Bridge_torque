/*
 * timer.c
 *
 *  Created on: 2017/05/18
 *      Author: lc
 */
#include "sys.h"
#include "gpio.h"
#include "timer.h"
#include "usart.h"
#include "setting.h"
#include "ads1258.h"
#include "usbh_usr.h"
#include "lcd12864.h"
#include "stmflash.h"
#include "initstate.h"
#include "fatfs_api.h"
#include "sdio_sdcard.h"

void TIM_Init(void)
{
    TIM2_Init();
    TIM3_Init();
    TIM4_Init();
    TIM5_Init();
    TIM7_Init();
    TIM13_Init();
    TIM14_Init();
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
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) //溢出中断
	{
        ads1258_SampleProc();
        //lcd12864_10us_proc();
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除中断标志位
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
        TIM4_set(0);
        USART2_RECV_Timeout();
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
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //清除中断标志位
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
        //LCD_Proc_5ms();
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

void TIM13_Init(void)//2s: for system reset
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13,ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler= 8400 - 1;
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM13,&TIM_TimeBaseInitStructure);

    TIM_ITConfig(TIM13,TIM_IT_Update,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel=TIM8_UP_TIM13_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM13_Enable(u8 state)
{
    if(state)
    {
        TIM_Cmd(TIM13,ENABLE);
    }
    else
    {
        TIM_Cmd(TIM13,DISABLE);
    }
    TIM13->CNT = 0;
}

void TIM8_UP_TIM13_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM13,TIM_IT_Update)==SET)
    {
        if(PIN_RESET == 0)
        {
            set_DevID(1001);
            set_CtrlState(0xFFFF);
            set_Frequent(200);
            set_SendTimeServer(1);
            set_SendTimeDynamic(1);
            flash_setValues(1001, 0xFFFF, 200, 1, 200);
        }
    }
    TIM_Cmd(TIM13,DISABLE);
    TIM_ClearITPendingBit(TIM13,TIM_IT_Update);
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

    TIM_Cmd(TIM14, ENABLE);
    TIM_ITConfig(TIM14,TIM_IT_Update,DISABLE);
}

void TIM14_IT_ENABLE(void)
{
    TIM_ITConfig(TIM14,TIM_IT_Update,ENABLE);
}

void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM14,TIM_IT_Update)==SET)
    {
        add_timestamp();
        //show_Update();
        SD_1s_CheckProc();
    }
    TIM_ClearITPendingBit(TIM14,TIM_IT_Update);
}

