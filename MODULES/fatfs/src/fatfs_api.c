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
	res=f_open(file,(const TCHAR*)path,mode);//���ļ���
	return res;
} 

//�ر��ļ�
//����ֵ:ִ�н��
u8 mf_close(void)
{
	f_close(file);
	return 0;
}

//��������
//len:�����ĳ���
//����ֵ:ִ�н��
u8 mf_read(u16 len)
{
	u16 i,t;
	u8 res=0;
	u16 tlen=0;
	LOG_DEBUG("\r\nRead file data is:\r\n");
	for(i=0;i<len/512;i++)
	{
		res=f_read(file,fatbuf,512,&br);
		if(res)
		{
			LOG_DEBUG("Read Error:%d\r\n",res);
			break;
		}else
		{
			tlen+=br;
			for(t=0;t<br;t++)
            {
                LOG_DEBUG("%c",fatbuf[t]); 
            }
		}
	}
	if(len%512)
	{
		res=f_read(file,fatbuf,len%512,&br);
		if(res)	//�����ݳ�����
		{
			LOG_DEBUG("\r\nRead Error:%d\r\n",res);   
		}else
		{
			tlen+=br;
			for(t=0;t<br;t++)
            {
                LOG_DEBUG("%c",fatbuf[t]); 
            }
		}	 
	}
	if(tlen)LOG_DEBUG("\r\nReaded data len:%d\r\n",tlen);//���������ݳ���
	LOG_DEBUG("Read data over\r\n");	 
	return res;
}

//д������
//dat:���ݻ�����
//len:д�볤��
//����ֵ:ִ�н��
u8 mf_write(u8*dat,u16 len)
{			    
	return f_write(file,dat,len,&bw);
}

//��Ŀ¼
 //path:·��
//����ֵ:ִ�н��
u8 mf_opendir(u8* path)
{
	return f_opendir(&dir,(const TCHAR*)path);	
}

//�ر�Ŀ¼ 
//����ֵ:ִ�н��
u8 mf_closedir(void)
{
	return f_closedir(&dir);	
}

//���ȡ�ļ���
//����ֵ:ִ�н��
u8 mf_readdir(void)
{
	u8 res;
	char *fn;			 
#if _USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN,fileinfo.lfsize);
#endif		  
	res=f_readdir(&dir,&fileinfo);//��ȡһ���ļ�����Ϣ
	if(res!=FR_OK || fileinfo.fname[0]==0)
	{
		myfree(SRAMIN,fileinfo.lfname);
		return res;//������.
	}
#if _USE_LFN
	fn=*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else
	fn=fileinfo.fname;;
#endif	
	LOG_DEBUG("\r\n DIR info:\r\n");

	LOG_DEBUG("dir.id:%d\r\n",dir.id);
	LOG_DEBUG("dir.index:%d\r\n",dir.index);
	LOG_DEBUG("dir.sclust:%d\r\n",dir.sclust);
	LOG_DEBUG("dir.clust:%d\r\n",dir.clust);
	LOG_DEBUG("dir.sect:%d\r\n",dir.sect);	  

	LOG_DEBUG("\r\n");
	LOG_DEBUG("File Name is:%s\r\n",fn);
	LOG_DEBUG("File Size is:%d\r\n",fileinfo.fsize);
	LOG_DEBUG("File data is:%d\r\n",fileinfo.fdate);
	LOG_DEBUG("File time is:%d\r\n",fileinfo.ftime);
	LOG_DEBUG("File Attr is:%d\r\n",fileinfo.fattrib);
	LOG_DEBUG("\r\n");
	myfree(SRAMIN,fileinfo.lfname);
	return 0;
}			 

 //�����ļ�
 //path:·��
 //����ֵ:ִ�н��
u8 mf_scan_files(u8 * path)
{
	FRESULT res;	  
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN,fileinfo.lfsize);
#endif		  

    res = f_opendir(&dir,(const TCHAR*)path); //��һ��Ŀ¼
    if (res == FR_OK) 
	{	
		LOG_DEBUG("\r\n"); 
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //������/��ĩβ��,�˳�
	        //if (fileinfo.fname[0] == '.') continue;             //�����ϼ�Ŀ¼
#if _USE_LFN
        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else							   
        	fn = fileinfo.fname;
#endif	                                              /* It is a file. */
			LOG_DEBUG("%s/", path);//��ӡ·��	
			LOG_DEBUG("%s\r\n",  fn);//��ӡ�ļ���	  
		} 
    }	  
	myfree(SRAMIN,fileinfo.lfname);
    return res;	  
}

//��ʾʣ������
//drv:�̷�
//����ֵ:ʣ������(�ֽ�)
u32 mf_showfree(u8 *drv)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res = f_getfree((const TCHAR*)drv,(DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//�õ���������
	    fre_sect = fre_clust * fs1->csize;			//�õ�����������	   
#if _MAX_SS!=512
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		if(tot_sect<20480)//������С��10M
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    LOG_DEBUG("\r\n����������:%d KB\r\n"
		           "���ÿռ�:%d KB\r\n",
		           tot_sect>>1,fre_sect>>1);
		}else
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    LOG_DEBUG("\r\n����������:%d MB\r\n"
		           "���ÿռ�:%d MB\r\n",
		           tot_sect>>11,fre_sect>>11);
		}
	}
	return fre_sect;
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

//����Ŀ¼
//pname:Ŀ¼·��+����
//����ֵ:ִ�н��
u8 mf_mkdir(u8*pname)
{
	return f_mkdir((const TCHAR *)pname);
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

//ɾ���ļ�/Ŀ¼
//pname:�ļ�/Ŀ¼·��+����
//����ֵ:ִ�н��
u8 mf_unlink(u8 *pname)
{
	return  f_unlink((const TCHAR *)pname);
}

//�޸��ļ�/Ŀ¼����(���Ŀ¼��ͬ,�������ƶ��ļ�Ŷ!)
//oldname:֮ǰ������
//newname:������
//����ֵ:ִ�н��
u8 mf_rename(u8 *oldname,u8* newname)
{
	return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
}

//��ȡ�̷����������֣�
//path:����·��������"0:"��"1:"  
void mf_getlabel(u8 *path)
{
	u8 buf[20];
	u32 sn=0;
	u8 res;
	res=f_getlabel ((const TCHAR *)path,(TCHAR *)buf,(DWORD*)&sn);
	if(res==FR_OK)
	{
		LOG_DEBUG("\r\n����%s ���̷�Ϊ:%s\r\n",path,buf);
		LOG_DEBUG("����%s �����к�:%X\r\n\r\n",path,sn); 
	}else LOG_DEBUG("\r\n��ȡʧ�ܣ�������:%X\r\n",res);
}

//�����̷����������֣����11���ַ�������֧�����ֺʹ�д��ĸ����Լ����ֵ�
//path:���̺�+���֣�����"0:ALIENTEK"��"1:OPENEDV"  
void mf_setlabel(u8 *path)
{
	u8 res;
	res=f_setlabel ((const TCHAR *)path);
	if(res==FR_OK)
	{
		LOG_DEBUG("\r\n�����̷����óɹ�:%s\r\n",path);
	}else LOG_DEBUG("\r\n�����̷�����ʧ�ܣ�������:%X\r\n",res);
} 

//���ļ������ȡһ���ַ���
//size:Ҫ��ȡ�ĳ���
void mf_gets(u16 size)
{
 	TCHAR* rbuf;
	rbuf=f_gets((TCHAR*)fatbuf,size,file);
	if(*rbuf==0)return  ;//û�����ݶ���
	else
	{
		LOG_DEBUG("\r\nThe String Readed Is:%s\r\n",rbuf);  	  
	}			    	
}

//��Ҫ_USE_STRFUNC>=1
//дһ���ַ����ļ�
//c:Ҫд����ַ�
//����ֵ:ִ�н��
u8 mf_putc(u8 c)
{
	return f_putc((TCHAR)c,file);
}

//д�ַ������ļ�
//c:Ҫд����ַ���
//����ֵ:д����ַ�������
u8 mf_puts(u8*c)
{
	return f_puts((TCHAR*)c,file);
}

void fatfs_init(void)
{
    u8 res = 0;
    u32 total,free;
				 
  	res = f_mount(fs[0],"0:",1); 					//����SD�� 
    LOG_DEBUG("RESULT : %d\r\n",res);												    
	while(exf_getfree("0",&total,&free))	//�õ�SD������������ʣ������
	{
		LOG_DEBUG("SD Card Fatfs Error!");
		delay_ms(200);
	}													  			    
	LOG_DEBUG("FATFS OK!\r\n");
    delay_ms(10);    
	LOG_DEBUG("SD Total Size:%d MB",total>>10);
    delay_ms(10);    
	LOG_DEBUG("SD  Free Size:%d MB",free>>10); 	    
    add_InitState(SDSTATE);
}













