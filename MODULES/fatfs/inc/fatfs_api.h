#ifndef __FATTESTER_H
#define __FATTESTER_H
#include <stm32f4xx.h>
#include "ff.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//FATFS ���Դ���
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
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





























