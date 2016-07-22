#include "main.h"


//这是一个注释
int main(void)
{
    u8 res;
    int i,j;
	SystemConfiguration();

    mf_scan_files("0:");
    
    for(j=0;j<8192;j++)
    {
        mf_open("0:/kky.txt",FA_WRITE);
        f_lseek(file,file->fsize);
        mf_write("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\
                  ccccccccccccccccccccccccccccccccccccdddddddddddddddddddddddddddddddddddddddd\
                   ",128);
    mf_close();
    delay_ms(50);
    }
    
//    mf_open("0:/kky.txt",FA_READ);
//    res=mf_read(10);
//    LOG_DEBUG("RES=%d\r\n",res);
//    mf_close();
    

    
	while(1)
	{
        delay_ms(50);
	}
}

void SystemConfiguration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
	USART1_Configuration();
    USART2_Configuration();
	USART3_Configuration();
    mymem_init(SRAMIN);    //初始化内部内存池
    mymem_init(SRAMCCM);   //初始化CCM内存池
    check_SD_Card();
}


