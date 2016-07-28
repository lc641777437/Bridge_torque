#include "gpio.h"

/*
STATE_1  PE5
STATE_2  PE4
STATE_3  PE3
STATE_4  PE2
STATE_5  PA11
STATE_6  PA10
STATE_7  PA9
STATE_8  PA8
STATE_9  PE15
STATE_10  PE14
STATE_11  PE13
STATE_12  PE12
STATE_13  PE11
STATE_14  PE10
STATE_15  PE9
STATE_16  PE8

GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)
*/
void GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = 0xFF3C;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

