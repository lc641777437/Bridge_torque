#ifndef __FATTESTER_H
#define __FATTESTER_H
#include <stm32f4xx.h>
#include "ff.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//FATFS 测试代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////


u8 mf_mount(u8* path,u8 mt);
u8 mf_open(u8*path,u8 mode);
u8 mf_close(void);
u8 mf_write(u8*dat,u16 len);
u8 mf_lseek(u32 offset);
u32 mf_tell(void);
u32 mf_size(void);

void fatfs_init_SD(void);
void fatfs_init_USB(void);

#endif





























