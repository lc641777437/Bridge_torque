#ifndef __RTC_H__
#define __RTC_H__
#include "stm32f4xx.h"

u8 My_RTC_Init(void);
void RTC_Set_WakeUp(u32 wksel,u16 cnt);
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec);
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week);

#endif/*__RTC_H__*/
