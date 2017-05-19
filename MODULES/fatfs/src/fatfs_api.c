#include "fatfs_api.h"
#include "sdio_sdcard.h"
#include "usart.h"
#include "exfuns.h"
#include "malloc.h"
#include "ff.h"
#include "delay.h"
#include "initstate.h"

//Ϊ����ע�Ṥ����
//path:����·��������"0:"��"1:"
//mt:0��������ע�ᣨ�Ժ�ע�ᣩ��1������ע��
//����ֵ:ִ�н��
u8 mf_mount(u8* path,u8 mt)
{
	return f_mount(fs[2],(const TCHAR*)path,mt);
}

//��·���µ��ļ�
//path:·��+�ļ���
//mode:��ģʽ
//����ֵ:ִ�н��
u8 mf_open(u8*path,u8 mode)
{
	u8 res;
	res=f_open(file, (const TCHAR*)path,mode);//���ļ���
	return res;
}

//�ر��ļ�
//����ֵ:ִ�н��
u8 mf_close(void)
{
	f_close(file);
	return 0;
}

//д������
//dat:���ݻ�����
//len:д�볤��
//����ֵ:ִ�н��
u8 mf_write(u8*dat,u16 len)
{
	return f_write(file,dat,len,&bw);
}

//�ļ���дָ��ƫ��
//offset:����׵�ַ��ƫ����
//����ֵ:ִ�н��.
u8 mf_lseek(u32 offset)
{
	return f_lseek(file,offset);
}

//��ȡ�ļ���ǰ��дָ���λ��.
//����ֵ:λ��
u32 mf_tell(void)
{
	return f_tell(file);
}

//��ȡ�ļ���С
//����ֵ:�ļ���С
u32 mf_size(void)
{
	return f_size(file);
}

//��ʽ��
//path:����·��������"0:"��"1:"
//mode:ģʽ
//au:�ش�С
//����ֵ:ִ�н��
u8 mf_fmkfs(u8* path,u8 mode,u16 au)
{
	return f_mkfs((const TCHAR*)path,mode,au);//��ʽ��,drv:�̷�;mode:ģʽ;au:�ش�С
}

void fatfs_init_SD(void)
{
    u8 res = 0;
    u32 total = 0, free = 0;

  	res = f_mount(fs[0], "0:", 1);//����SD��
    LOG_DEBUG("RESULT: %d\r\n", res);
	while(exf_getfree("0", &total, &free))//�õ�SD������������ʣ������
	{
		LOG_DEBUG("SD Card Fatfs Error!");
		delay_ms(200);
	}
	LOG_DEBUG("FATFS OK!\r\n");
	LOG_DEBUG("SD Total Size:%d MB",total>>10);
	LOG_DEBUG("SD  Free Size:%d MB",free>>10);
    set_DeviceState(DEVICE_FATFS);
}

void fatfs_init_USB(void)
{
}


