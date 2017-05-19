#include "fatfs_api.h"
#include "sdio_sdcard.h"
#include "usart.h"
#include "exfuns.h"
#include "malloc.h"
#include "ff.h"
#include "delay.h"
#include "initstate.h"

//为磁盘注册工作区
//path:磁盘路径，比如"0:"、"1:"
//mt:0，不立即注册（稍后注册）；1，立即注册
//返回值:执行结果
u8 mf_mount(u8* path,u8 mt)
{
	return f_mount(fs[2],(const TCHAR*)path,mt);
}

//打开路径下的文件
//path:路径+文件名
//mode:打开模式
//返回值:执行结果
u8 mf_open(u8*path,u8 mode)
{
	u8 res;
	res=f_open(file, (const TCHAR*)path,mode);//打开文件夹
	return res;
}

//关闭文件
//返回值:执行结果
u8 mf_close(void)
{
	f_close(file);
	return 0;
}

//写入数据
//dat:数据缓存区
//len:写入长度
//返回值:执行结果
u8 mf_write(u8*dat,u16 len)
{
	return f_write(file,dat,len,&bw);
}

//文件读写指针偏移
//offset:相对首地址的偏移量
//返回值:执行结果.
u8 mf_lseek(u32 offset)
{
	return f_lseek(file,offset);
}

//读取文件当前读写指针的位置.
//返回值:位置
u32 mf_tell(void)
{
	return f_tell(file);
}

//读取文件大小
//返回值:文件大小
u32 mf_size(void)
{
	return f_size(file);
}

//格式化
//path:磁盘路径，比如"0:"、"1:"
//mode:模式
//au:簇大小
//返回值:执行结果
u8 mf_fmkfs(u8* path,u8 mode,u16 au)
{
	return f_mkfs((const TCHAR*)path,mode,au);//格式化,drv:盘符;mode:模式;au:簇大小
}

void fatfs_init_SD(void)
{
    u8 res = 0;
    u32 total = 0, free = 0;

  	res = f_mount(fs[0], "0:", 1);//挂载SD卡
    LOG_DEBUG("RESULT: %d\r\n", res);
	while(exf_getfree("0", &total, &free))//得到SD卡的总容量和剩余容量
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


