#include "lcd12864.h"
#include "delay.h"

void lcd12864_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //PC2 RS,PC3 RW
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //PA0 EN
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //PA3-PA6   DB0-DB3
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //PB0-PB1   DB4-DB5
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //PF11-PF12  DB6-DB7
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    
}

//u8 LCD12864_busy(void)
//{
//	u8 x;
//	LCD_RS=0; 
//	LCD_RW=1;  
//	LCD_EN=1; 
//	x= PFin(12);
//	LCD_EN=0; 
//	return x;
//}

void DataPort_Write(u8 data)
{
    LCD_DB0=data&0x01?1:0;
    LCD_DB1=data&0x02?1:0;
    LCD_DB2=data&0x04?1:0;
    LCD_DB3=data&0x08?1:0;
    LCD_DB4=data&0x10?1:0;
    LCD_DB5=data&0x20?1:0;
    LCD_DB6=data&0x40?1:0;
    LCD_DB7=data&0x80?1:0;
}

void Write_Com(u8 com)
{
    delay_ms(5);
    LCD_RS=0;
    LCD_RW=0;
    LCD_EN=0;
    delay_us(10);
    DataPort_Write(com);
    delay_us(10);
    LCD_EN=1;
    delay_us(10);
    LCD_EN=0;
}

void Write_Data(u8 dat)
{
    delay_ms(5);
    LCD_RS=1;
    LCD_RW=0;
    LCD_EN=0;
    delay_us(10);
    DataPort_Write(dat);
    delay_us(10);
    LCD_EN=1;
    delay_us(10);
    LCD_EN=0;
}

void Clear_Screen(void)
{
    Write_Com(0x30);
    delay_ms(5);
    Write_Com(0x34);
    delay_ms(5);
    Write_Com(0x01);
}

void LCD_Init(void)
{
    Write_Com(0x34);
    delay_ms(5);
    Write_Com(0x30);
    delay_ms(5);
    Write_Com(0x0c);
    delay_ms(5);
    Write_Com(0x01);
    delay_ms(5);
}

void Write_String(u8 y,u8 x,u8 *s)
{
    switch(y)
    {
        case 0:Write_Com(0x80+x);break;
        case 1:Write_Com(0x90+x);break;
        case 2:Write_Com(0x88+x);break;
        case 3:Write_Com(0x98+x);break;
        default:break;
    }
    while(*s)
    {
        Write_Data(*s);
        s++;
    }
}
