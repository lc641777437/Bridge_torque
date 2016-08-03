#include "spi.h"
#include "ads1258.h"

static u8 spi_Count_S;
static u8 iData[4];
extern int Count;

void SPI1_Init(void)
{	 
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1); 
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1); 
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);


    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//��λSPI1
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//ֹͣ��λSPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����ģʽ��˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//SPI����ģʽ:��SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI���ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//����ͬ��ʱ�ӿ���״̬
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//���ݲ�����������
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//������Ԥ��ƵֵΪ256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ�ļ������ʽ
    SPI_Init(SPI1, &SPI_InitStructure);  //��ʼ��SPI�Ĵ���

    SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
    SPI1_ReadWriteByte(0xff);//��������
}   

//SPI1�ٶ����ú���
//SPI�ٶ�=fAPB2/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2һ��Ϊ84Mhz:
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
    SPI1->CR1&=0XFFC7;//λ3-5����
    SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ�
    SPI_Cmd(SPI1,ENABLE); //ʹ��SPI1
} 

//SPI1��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{		 			 
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������
    SPI_I2S_SendData(SPI1, TxData); //��������
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�����һ��byte
    return SPI_I2S_ReceiveData(SPI1); //���ؽ��յ�����
}

void SPI2_Init(void)
{	 
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2); 
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2); 
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);


    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);//��λSPI1
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//ֹͣ��λSPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����ģʽ��˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//SPI����ģʽ:��SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI���ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//����ͬ��ʱ�ӿ���״̬
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//���ݲ�����������
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//������Ԥ��ƵֵΪ256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ�ļ������ʽ
    SPI_Init(SPI2, &SPI_InitStructure);  //��ʼ��SPI�Ĵ���
    
    NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
    
//    SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_RXNE,ENABLE);
//    SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_TXE,ENABLE);

    SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����
}   

//SPI1�ٶ����ú���
//SPI�ٶ�=fAPB2/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2һ��Ϊ84Mhz:
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
    SPI2->CR1&=0XFFC7;//λ3-5����
    SPI2->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ�
    SPI_Cmd(SPI2,ENABLE); //ʹ��SPI1
} 


//SPI1��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{		 			 
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������
    SPI_I2S_SendData(SPI2, TxData); //��������
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�����һ��byte
    return SPI_I2S_ReceiveData(SPI2); //���ؽ��յ�����
}

void SPI2_IRQHandler(void)
{    
    if(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==SET)
    {
        if(spi_Count_S==0)
        {
            SPI_I2S_SendData(SPI2,0xA0);
            spi_Count_S++;
        }
        else if(spi_Count_S<4)
        {
            iData[spi_Count_S-1]=SPI_I2S_ReceiveData(SPI2);
            SPI_I2S_SendData(SPI2,0xA0);
            spi_Count_S++;
        }
        else
        {
            iData[spi_Count_S-1]=SPI_I2S_ReceiveData(SPI2);
            spi_Count_S=0;
            iData[0]&=0x1f;
            Count++;
            ad_DataConvert(iData);
            SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_TXE,DISABLE);
        }
    }
}
