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

u8 isLCDBUSY(void);

u8 isLCDEN(void);
void LCD_Start_10us(u8 flag);
void LCD_Proc_10us(void);

void LCD_Start_5ms(u8 flag);
void LCD_Proc_5ms(void);
void show_Update(void);

#define SENSOR_1 0,0
#define SENSOR_2 0,2
#define SENSOR_3 0,4
#define SENSOR_4 0,6
#define SENSOR_5 1,0
#define SENSOR_6 1,2
#define SENSOR_7 1,4
#define SENSOR_8 1,6
#define SENSOR_9 2,0
#define SENSOR_10 2,2
#define SENSOR_11 2,4
#define SENSOR_12 2,6
#define SENSOR_13 3,0
#define SENSOR_14 3,2
#define SENSOR_15 3,4
#define SENSOR_16 3,6

#endif
