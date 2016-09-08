#ifndef __LCD12864_H
#define __LCD12864_H

#include "stm32f4xx.h"
#include "sys.h"
enum
{
    LCD_NULL,
    LCD_DATA,
    LCD_END
};
#define LCD_RS PCout(2)
#define LCD_RW PCout(3)
#define LCD_EN PAout(0)
#define LCD_DB0 PAout(3)
#define LCD_DB1 PAout(4)
#define LCD_DB2 PAout(5)
#define LCD_DB3 PAout(6)
#define LCD_DB4 PBout(0)
#define LCD_DB5 PBout(1)
#define LCD_DB6 PFout(11)
#define LCD_DB7 PFout(12)

void lcd12864_GPIO_Init(void);
void DataPort_Write(u8 data);
void Write_Com(u8 com);
void Write_Data(u8 dat);
void Clear_Screen(void);
void LCD_Init(void);
void ShowString(u8 line, u8 pos, u8 *s);


u8 isLCDEN(void);
void LCD_Start_10us(u8 flag);
void LCD_Proc_10us(void);

void LCD_Start_5ms(u8 flag);
void LCD_Proc_5ms(void);

#endif
