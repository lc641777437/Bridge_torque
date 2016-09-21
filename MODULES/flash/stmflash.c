#include "stmflash.h"
#include "delay.h"
#include "usart.h" 
#include "gpio.h"
#include "tcp_client_demo.h" 
#include "timer.h"

static u32 Flash_State[4];//0:DEV_ID   1:CTRL_STATE   2:IP_ADDRESS   3:FREQUENCE

//��ȡָ����ַ�İ���(16λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}  
//��ȡĳ����ַ���ڵ�flash����
//addr:flash��ַ
//����ֵ:0~11,��addr���ڵ�����
uint16_t STMFLASH_GetFlashSector(u32 addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_Sector_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_Sector_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_Sector_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_Sector_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_Sector_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_Sector_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_Sector_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_Sector_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_Sector_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_Sector_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_Sector_10; 
	return FLASH_Sector_11;	
}
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ر�ע��:��ΪSTM32F4������ʵ��̫��,û�취���ر�����������,���Ա�����
//         д��ַ�����0XFF,��ô���Ȳ������������Ҳ�������������.����
//         д��0XFF�ĵ�ַ,�����������������ݶ�ʧ.����д֮ǰȷ��������
//         û����Ҫ����,��������������Ȳ�����,Ȼ����������д. 
//�ú�����OTP����Ҳ��Ч!��������дOTP��!
//OTP�����ַ��Χ:0X1FFF7800~0X1FFF7A0F
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ4�ı���!!)
//pBuffer:����ָ��
//NumToWrite:��(32λ)��(����Ҫд���32λ���ݵĸ���.) 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
  FLASH_Status status = FLASH_COMPLETE;
	u32 addrx=0;
	u32 endaddr=0;	
  if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//�Ƿ���ַ
	FLASH_Unlock();									//���� 
  FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���
 		
	addrx=WriteAddr;				//д�����ʼ��ַ
	endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
	if(addrx<0X1FFF0000)			//ֻ�����洢��,����Ҫִ�в�������!!
	{
		while(addrx<endaddr)		//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
		{
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//�з�0XFFFFFFFF�ĵط�,Ҫ�����������
			{   
				status=FLASH_EraseSector(STMFLASH_GetFlashSector(addrx),VoltageRange_3);//VCC=2.7~3.6V֮��!!
				if(status!=FLASH_COMPLETE)break;	//����������
			}else addrx+=4;
		} 
	}
	if(status==FLASH_COMPLETE)
	{
		while(WriteAddr<endaddr)//д����
		{
			if(FLASH_ProgramWord(WriteAddr,*pBuffer)!=FLASH_COMPLETE)//д������
			{ 
				break;	//д���쳣
			}
			WriteAddr+=4;
			pBuffer++;
		} 
	}
  FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
	FLASH_Lock();//����
} 

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToRead:��(4λ)��
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//��ȡ4���ֽ�.
		ReadAddr+=4;//ƫ��4���ֽ�.	
	}
}


void My_Flash_Read(void)
{
    STMFLASH_Read(FALSH_SAVE_ADDR,Flash_State,4);
    set_CtrlState(Flash_State[1]);
    if(Flash_State[3]<500)
    {
        set_Frequent(Flash_State[3]);
    }
    else 
    {
        set_Frequent(200);
    }
}

u32 get_FlashState(int i)
{
    if(i>=0&&i<=3)
    {
        return Flash_State[i];
    }
    else 
    {
        return 0;
    }
}

void Write_DeviceID(u32 DeviceID)
{
    Flash_State[0]=DeviceID;
    STMFLASH_Write(FALSH_SAVE_ADDR,Flash_State,4);
}

void Write_CtrlState(u32 CtrlState)
{
    Flash_State[1]=CtrlState;
    STMFLASH_Write(FALSH_SAVE_ADDR,Flash_State,4);
}

void Write_IPAddress(u32 IPAddress)
{
    Flash_State[2]=IPAddress;
    STMFLASH_Write(FALSH_SAVE_ADDR,Flash_State,4);
}

void Write_Frequent(u32 Frequent)
{
    Flash_State[3]=Frequent;
    STMFLASH_Write(FALSH_SAVE_ADDR,Flash_State,4);
}

void set_CtrlState(u32 CtrlState)
{
    CTRL_0=(CtrlState>>0)&1;
    CTRL_1=(CtrlState>>1)&1;
    CTRL_2=(CtrlState>>2)&1;
    CTRL_3=(CtrlState>>3)&1;
    CTRL_4=(CtrlState>>4)&1;
    CTRL_5=(CtrlState>>5)&1;
    CTRL_6=(CtrlState>>6)&1;
    CTRL_7=(CtrlState>>7)&1;
    CTRL_8=(CtrlState>>8)&1;
    CTRL_9=(CtrlState>>9)&1;
    CTRL_10=(CtrlState>>10)&1;
    CTRL_11=(CtrlState>>11)&1;
    CTRL_12=(CtrlState>>12)&1;
    CTRL_13=(CtrlState>>13)&1;
    CTRL_14=(CtrlState>>14)&1;
    CTRL_15=(CtrlState>>15)&1;
}






