#include "diskio.h"		/* FatFs lower layer API */
#include "sdio_sdcard.h"
#include "malloc.h"		
#include "usbh_usr.h"


#define SD_CARD	 0  //SD��,���Ϊ0
#define USB_DISK 1	//�ⲿflash,���Ϊ1

//��ʼ������
DSTATUS disk_initialize (BYTE pdrv)/* Physical drive nmuber (0..) */
{
	u8 res=0;	    
	switch(pdrv)
	{
		case SD_CARD://SD��
			res=SD_Init();//SD����ʼ�� 
  			break;
        case USB_DISK://�ⲿflash
	  		if(USBH_UDISK_Status())return 0;	//U�����ӳɹ�,�򷵻�1.���򷵻�0		  
			else return 1;	 
		default:
			res=1; 
	}		 
	if(res)
        return STA_NOINIT;
	else 
        return 0; //��ʼ���ɹ�
}  

//������
//drv:���̱��0~9
//*buff:���ݽ��ջ����׵�ַ
//sector:������ַ
//count:��Ҫ��ȡ��������
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	u8 res=0; 
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)
	{
		case SD_CARD://SD��
			res=SD_ReadDisk(buff,sector,count);	 
			while(res)//������
			{
				SD_Init();	//���³�ʼ��SD��
				res=SD_ReadDisk(buff,sector,count);	
				//printf("sd rd error:%d\r\n",res);
			}
			break;
        case USB_DISK://U�� 
			res=USBH_UDISK_Read(buff,sector,count);	  
			break;
		default:
			res=1; 
	}
   //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res==0x00)
        return RES_OK;	 
    else 
        return RES_ERROR;	   
}

//д����
//drv:���̱��0~9
//*buff:���������׵�ַ
//sector:������ַ
//count:��Ҫд���������
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	u8 res=0;  
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)
	{
		case SD_CARD://SD��
			res=SD_WriteDisk((u8*)buff,sector,count);
			while(res)//д����
			{
				SD_Init();	//���³�ʼ��SD��
				res=SD_WriteDisk((u8*)buff,sector,count);	
				//printf("sd wr error:%d\r\n",res);
			}
			break;
		case USB_DISK://U��
			res=USBH_UDISK_Write((u8*)buff,sector,count); 
			break;
		default:
			res=1; 
	}
    //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res == 0x00)
        return RES_OK;	 
    else 
        return RES_ERROR;	
}
#endif


//����������Ļ��
 //drv:���̱��0~9
 //ctrl:���ƴ���
 //*buff:����/���ջ�����ָ��
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;						  			     
	if(pdrv==SD_CARD)//SD��
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512; 
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
				*(WORD*)buff = SDCardInfo.CardBlockSize;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SDCardInfo.CardCapacity/512;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}
    else if(pdrv==USB_DISK)	//U��
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff=512;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff=512;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff=USBH_MSC_Param.MSCapacity;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }		
	}
    else 
        res=RES_ERROR;//�����Ĳ�֧��
    return res;
}
#endif


DWORD get_fattime (void)
{				 
	return 0;
}		

DSTATUS disk_status (BYTE pdrv)
{
    return 0;
}

//��̬�����ڴ�
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(SRAMIN,size);
}
//�ͷ��ڴ�
void ff_memfree (void* mf)		 
{
	myfree(SRAMIN,mf);
}

















