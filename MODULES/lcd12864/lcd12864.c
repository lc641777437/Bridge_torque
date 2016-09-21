#include "lcd12864.h"
#include "delay.h"
#include "timer.h"
#include "gpio.h"

static u8 LcdWriteFlag;
static u8 lcd_data[17];
static u8 pos;
static u8 isLCDBusy;
static u8 show_Ctrl[16];  // 0:I  1:V
static u8 show_State[16]; // 0:N  1:Y
static u8 show_Count;

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
    LCD_RS=0;
    LCD_RW=0;
    LCD_EN=0;
    DataPort_Write(com);
    LCD_EN=1;
}

void Write_Data(u8 dat)
{
    LCD_RS=1;
    LCD_RW=0;
    LCD_EN=0;
    DataPort_Write(dat);
    LCD_EN=1;
}

void Clear_Screen(void)
{
    Write_Com(0x01);
}

void LCD_Init(void)
{
    int i,j;
    char string[17];
    
    Write_Com(0x34);
    delay_ms(5);
    Write_Com(0x30);
    delay_ms(5);
    Write_Com(0x0c);
    delay_ms(5);
    Write_Com(0x01);
    delay_ms(5);
    
    show_State[0]=STATE_0;
    show_State[1]=STATE_1;
    show_State[2]=STATE_2;
    show_State[3]=STATE_3;
    show_State[4]=STATE_4;
    show_State[5]=STATE_5;
    show_State[6]=STATE_6;
    show_State[7]=STATE_7;
    show_State[8]=STATE_8;
    show_State[9]=STATE_9;
    show_State[10]=STATE_10;
    show_State[11]=STATE_11;
    show_State[12]=STATE_12;
    show_State[13]=STATE_13;
    show_State[14]=STATE_14;
    show_State[15]=STATE_15;
    
    show_Ctrl[0]=CTRL_0;
    show_Ctrl[1]=CTRL_1;
    show_Ctrl[2]=CTRL_2;
    show_Ctrl[3]=CTRL_3;
    show_Ctrl[4]=CTRL_4;
    show_Ctrl[5]=CTRL_5;
    show_Ctrl[6]=CTRL_6;
    show_Ctrl[7]=CTRL_7;
    show_Ctrl[8]=CTRL_8;
    show_Ctrl[9]=CTRL_9;
    show_Ctrl[10]=CTRL_10;
    show_Ctrl[11]=CTRL_11;
    show_Ctrl[12]=CTRL_12;
    show_Ctrl[13]=CTRL_13;
    show_Ctrl[14]=CTRL_14;
    show_Ctrl[15]=CTRL_15;
    
    for(i=0;i<4;i++)
    {
        memset(string,'\0',17*sizeof(char));
        for(j=0;j<4;j++)
        {
            if(show_Ctrl[4*i+j]==0)
            {
                string[j * 4 + 0] = 'I';
            }
            else
            {
                string[j * 4 + 0] = 'V';
            }
            if(show_State[4*i+j]==0)
            {
                string[j * 4 + 1] = ' ';
            }
            else
            {
                string[j * 4 + 1] = '\x04';
            }
            string[j * 4 + 2] = ' ';
            string[j * 4 + 3] = ' ';
        }
        ShowString(i,0,(u8*)string);
        delay_ms(100);
    }
}

void ShowString(u8 line, u8 pos, u8 *s)
{
    if(isLCDBusy)return;
    
    switch(line)
    {   
        case 0:Write_Com(0x80+pos);break;
        case 1:Write_Com(0x90+pos);break;
        case 2:Write_Com(0x88+pos);break;
        case 3:Write_Com(0x98+pos);break;
    }
    strncpy((char *)lcd_data, (char *)s, 17);
    LcdWriteFlag = LCD_DATA;
    TIM7_Enable(ENABLE);
}

u8 isLCDEN(void)
{
    return LCD_EN;
}

u8 isLCDBUSY(void)
{
    return isLCDBusy;
}

void LCD_Proc_10us(void)
{
    LCD_EN = 0;
}

void LCD_Proc_5ms(void)
{
    if(LcdWriteFlag == LCD_DATA)
    {
        Write_Data(lcd_data[pos++]);
        if(pos > strlen((char *)lcd_data) - 1)
        {
            pos = 0;
            isLCDBusy = 0;
            TIM7_Enable(DISABLE);
            LcdWriteFlag = LCD_NULL;
            memset(lcd_data,'\0',16);
        }
    }
}

void show_Update(void)
{
    char isUpdate=0;
    switch(show_Count)
    {
        case 0:
            if(show_Ctrl[0]!=CTRL_0||show_State[0]!=STATE_0)
            {
                show_Ctrl[0]=CTRL_0;
                show_State[0]=STATE_0;
                isUpdate=1;
            }
            break;
        case 1:
            if(show_Ctrl[1]!=CTRL_1||show_State[1]!=STATE_1)
            {
                show_Ctrl[1]=CTRL_1;
                show_State[1]=STATE_1;
                isUpdate=1;
            }
            break;
        case 2:
            if(show_Ctrl[2]!=CTRL_2||show_State[2]!=STATE_2)
            {
                show_Ctrl[2]=CTRL_2;
                show_State[2]=STATE_2;
                isUpdate=1;
            }
            break;
        case 3:
            if(show_Ctrl[3]!=CTRL_3||show_State[3]!=STATE_3)
            {
                show_Ctrl[3]=CTRL_3;
                show_State[3]=STATE_3;
                isUpdate=1;
            }
            break;
        case 4:
            if(show_Ctrl[4]!=CTRL_4||show_State[4]!=STATE_4)
            {
                show_Ctrl[4]=CTRL_4;
                show_State[4]=STATE_4;
                isUpdate=1;
            }
            break;
        case 5:
            if(show_Ctrl[5]!=CTRL_5||show_State[5]!=STATE_5)
            {
                show_Ctrl[5]=CTRL_5;
                show_State[5]=STATE_5;
                isUpdate=1;
            }
            break;
        case 6:
            if(show_Ctrl[6]!=CTRL_6||show_State[6]!=STATE_6)
            {
                show_Ctrl[6]=CTRL_6;
                show_State[6]=STATE_6;
                isUpdate=1;
            }
            break;
        case 7:
            if(show_Ctrl[7]!=CTRL_7||show_State[7]!=STATE_7)
            {
                show_Ctrl[7]=CTRL_7;
                show_State[7]=STATE_7;
                isUpdate=1;
            }
            break;
        case 8:
            if(show_Ctrl[8]!=CTRL_8||show_State[8]!=STATE_8)
            {
                show_Ctrl[8]=CTRL_8;
                show_State[8]=STATE_8;
                isUpdate=1;
            }
            break;
        case 9:
            if(show_Ctrl[9]!=CTRL_9||show_State[9]!=STATE_9)
            {
                show_Ctrl[9]=CTRL_9;
                show_State[9]=STATE_9;
                isUpdate=1;
            }
            break;
        case 10:
            if(show_Ctrl[10]!=CTRL_10||show_State[10]!=STATE_10)
            {
                show_Ctrl[10]=CTRL_10;
                show_State[10]=STATE_10;
                isUpdate=1;
            }
            break;
        case 11:
            if(show_Ctrl[11]!=CTRL_11||show_State[11]!=STATE_11)
            {
                show_Ctrl[11]=CTRL_11;
                show_State[11]=STATE_11;
                isUpdate=1;
            }
            break;
        case 12:
            if(show_Ctrl[12]!=CTRL_12||show_State[12]!=STATE_12)
            {
                show_Ctrl[12]=CTRL_12;
                show_State[12]=STATE_12;
                isUpdate=1;
            }
            break;
        case 13:
            if(show_Ctrl[13]!=CTRL_13||show_State[13]!=STATE_13)
            {
                show_Ctrl[13]=CTRL_13;
                show_State[13]=STATE_13;
                isUpdate=1;
            }
            break;
        case 14:
            if(show_Ctrl[14]!=CTRL_14||show_State[14]!=STATE_14)
            {
                show_Ctrl[14]=CTRL_14;
                show_State[14]=STATE_14;
                isUpdate=1;
            }
            break;
        case 15:
            if(show_Ctrl[15]!=CTRL_15||show_State[15]!=STATE_15)
            {
                show_Ctrl[15]=CTRL_15;
                show_State[15]=STATE_15;
                isUpdate=1;
            }
            break;
        default:
            break;
    }
    if(isUpdate==1)
    {
        if(show_Ctrl[show_Count]==0&&show_State[show_Count]==0)
        {
            ShowString(show_Count/4,(show_Count%4)*2,"IN  ");
        }
        else if(show_Ctrl[show_Count]==0&&show_State[show_Count]==1)
        {
            ShowString(show_Count/4,(show_Count%4)*2,"IY  ");
        }
        else if(show_Ctrl[show_Count]==1&&show_State[show_Count]==0)
        {
            ShowString(show_Count/4,(show_Count%4)*2,"VN  ");
        }
        else if(show_Ctrl[show_Count]==1&&show_State[show_Count]==1)
        {
            ShowString(show_Count/4,(show_Count%4)*2,"VY  ");
        }
    }
    if(show_Count<15)
    {
        show_Count++;
    }
    else 
    {
        show_Count=0;
    }
}
