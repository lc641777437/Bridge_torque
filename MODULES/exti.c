#include "spi.h"
#include "sys.h"
#include "rtc.h"
#include "gpio.h"
#include "exti.h"
#include "usart.h"
#include "timer.h"
#include "delay.h"
#include "setting.h"
#include "ads1258.h"

u8 sample_count = 0;


void EXTI_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);

    /*AD DRDY EXTI*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;//nDRDY exti
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_Init(GPIOD,&GPIO_InitStructure);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD,EXTI_PinSource8);

    /*SYSTEM RESET EXTI*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_Init(GPIOD,&GPIO_InitStructure);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD,EXTI_PinSource9);

    /*DYNAMIC SAMPLE EXTI*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9 | GPIO_Pin_10;//DYNAMIC exti
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG,EXTI_PinSource10);


    EXTI_InitStructure.EXTI_Line= EXTI_Line8 | EXTI_Line9 |EXTI_Line10;
    EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;// Falling edge
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel=EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel=EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line8) != RESET)// // nDRDY for AD sample OK
    {
        if(START == 1)
        {
            if(sample_count < ADS1258_CHANNEL_NUM)
            {
                ads1258_ReadData();
            }
            else
            {
                START = 0;
                sample_count = 0;
                upgradeBufferSended2Pc();
                if(get_Send_Flag() == SEND_BY_UART1)
                {
                    ads1258_SendDataBy232();
                }
                else if(get_Send_Flag() == SEND_BY_UART3)
                {
                    ads1258_SendDataBy433();
                }
                set_Save_Flag(1);
            }
        }
		EXTI_ClearITPendingBit(EXTI_Line8);
    }

    if(EXTI_GetITStatus(EXTI_Line9) != RESET)// system reset
    {
        if(PIN_RESET == 0)
        {
            TIM13_Enable(1);
        }

        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}

void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line10) != RESET)// system reset
    {
        if(PIN_TRIGGER == 0)
        {
            set_isDynamic(1);
        }
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
}

