#include "spi.h"
#include "ads1258.h"
#include "exti.h"

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


    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//复位SPI1
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//停止复位SPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //数据模式：双向双线全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//SPI工作模式:主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//串行同步时钟空闲状态
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//数据采样的跳变沿
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//波特率预分频值为256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值的计算多项式
    SPI_Init(SPI1, &SPI_InitStructure);  //初始化SPI寄存器

    SPI_Cmd(SPI1, ENABLE); //使能SPI外设
    SPI1_ReadWriteByte(0xff);//启动传输
}

//SPI1速度设置函数
//SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256
//fAPB2一般为84Mhz:
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    SPI1->CR1&=0XFFC7;//位3-5清零
    SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度
    SPI_Cmd(SPI1,ENABLE); //使能SPI1
}

//SPI1读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空
    SPI_I2S_SendData(SPI1, TxData); //发送数据
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收一个byte
    return SPI_I2S_ReceiveData(SPI1); //返回接收的数据
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


    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);//复位SPI1
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//停止复位SPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //数据模式：双向双线全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//SPI工作模式:主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//串行同步时钟空闲状态
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//数据采样的跳变沿
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//波特率预分频值为256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值的计算多项式
    SPI_Init(SPI2, &SPI_InitStructure);  //初始化SPI寄存器

    NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    SPI_Cmd(SPI2, ENABLE); //使能SPI外设
}

//SPI1速度设置函数
//SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256
//fAPB2一般为84Mhz:
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    SPI2->CR1&=0XFFC7;//位3-5清零
    SPI2->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度
    SPI_Cmd(SPI2,ENABLE); //使能SPI1
}

//SPI1读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空
    SPI_I2S_SendData(SPI2, TxData); //发送数据
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收一个byte
    return SPI_I2S_ReceiveData(SPI2); //返回接收的数据
}

void SPI2_IRQHandler(void)
{
    static u8 iData[4];
    static u8 spi_Count_S;
    if(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==SET)
    {
        if(spi_Count_S == 0)
        {
            SPI_I2S_SendData(SPI2,0xA0);
            spi_Count_S++;
        }
        else if(spi_Count_S < 4)
        {
            iData[spi_Count_S - 1]=SPI_I2S_ReceiveData(SPI2);
            SPI_I2S_SendData(SPI2, 0xA0);
            spi_Count_S++;
        }
        else
        {
            iData[spi_Count_S - 1] = SPI_I2S_ReceiveData(SPI2);
            spi_Count_S = 0;
            iData[0] &= 0x1f;
            sample_count++;
            ads1258_storeOneChannelData(iData);// one channel value into ad adta
            SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
        }
    }
}
