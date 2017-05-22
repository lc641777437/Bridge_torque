#include "rtc.h"
#include "delay.h"
#include "ads1258.h"


//RTCʱ������
//hour,min,sec:Сʱ,����,����
//ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
//����ֵ:SUCEE(1),�ɹ�
//       ERROR(0),�����ʼ��ģʽʧ��
static ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
    RTC_TimeTypeDef RTC_TimeTypeInitStructure;

    RTC_TimeTypeInitStructure.RTC_Hours=hour;
    RTC_TimeTypeInitStructure.RTC_Minutes=min;
    RTC_TimeTypeInitStructure.RTC_Seconds=sec;
    RTC_TimeTypeInitStructure.RTC_H12=ampm;

    return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);

}
//RTC��������
//year,month,date:��(0~99),��(1~12),��(0~31)
//week:����(1~7,0,�Ƿ�!)
//����ֵ:SUCEE(1),�ɹ�
//       ERROR(0),�����ʼ��ģʽʧ��
static ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{

    RTC_DateTypeDef RTC_DateTypeInitStructure;
    RTC_DateTypeInitStructure.RTC_Date=date;
    RTC_DateTypeInitStructure.RTC_Month=month;
    RTC_DateTypeInitStructure.RTC_WeekDay=week;
    RTC_DateTypeInitStructure.RTC_Year=year;
    return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}

//�����Ի��Ѷ�ʱ������
/*wksel:  @ref RTC_Wakeup_Timer_Definitions
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
*/
//cnt:�Զ���װ��ֵ.����0,�����ж�.
static void RTC_Set_WakeUp(u32 wksel,u16 cnt)//1min
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    RTC_WakeUpCmd(DISABLE);//�ر�WAKE UP

    RTC_WakeUpClockConfig(wksel);//����ʱ��ѡ��

    RTC_SetWakeUpCounter(cnt);//����WAKE UP�Զ���װ�ؼĴ���

    RTC_ClearITPendingBit(RTC_IT_WUT); //���RTC WAKE UP�ı�־
    EXTI_ClearITPendingBit(EXTI_Line22);//���LINE22�ϵ��жϱ�־λ

    RTC_ITConfig(RTC_IT_WUT,ENABLE);//����WAKE UP ��ʱ���ж�

    RTC_WakeUpCmd( ENABLE);//����WAKE UP ��ʱ����

    EXTI_InitStructure.EXTI_Line = EXTI_Line22;//LINE22
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE22
    EXTI_Init(&EXTI_InitStructure);//����


    NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);//����
}

//RTC WAKE UP�жϷ�����
void RTC_WKUP_IRQHandler(void)//1min
{
    if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WAKE_UP�ж�
    {
        upgrateBufferSended2Sim808();
        ads1258_SendDataBy808();

        RTC_ClearFlag(RTC_FLAG_WUTF);   //����жϱ�־
    }
    EXTI_ClearITPendingBit(EXTI_Line22);//����ж���22���жϱ�־
}

//RTC��ʼ��
//����ֵ:0,��ʼ���ɹ�;
//       1,LSE����ʧ��;
//       2,�����ʼ��ģʽʧ��;
u8 My_RTC_Init(void)
{
    RTC_InitTypeDef RTC_InitStructure;
    u16 retry=0X1FFF;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ��PWRʱ��
    PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ�������

    if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//�Ƿ��һ������?
    {
        RCC_LSEConfig(RCC_LSE_ON);//LSE ����
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//���ָ����RCC��־λ�������,�ȴ����پ������
        {
            retry++;
            delay_ms(10);
        }
        if(retry==0)return 1;		//LSE ����ʧ��.

        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��
        RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ��

        RTC_InitStructure.RTC_AsynchPrediv = 0x1F;//RTC�첽��Ƶϵ��(1~0X7F)
        RTC_InitStructure.RTC_SynchPrediv  = 0x3FF;//RTCͬ����Ƶϵ��(0~7FFF)
        RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC����Ϊ,24Сʱ��ʽ
        RTC_Init(&RTC_InitStructure);

        RTC_Set_Time(0,0,0,RTC_H12_AM);	//����ʱ��
        RTC_Set_Date(0,1,1,1);		//�������� 2000-01-01

        RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//����Ѿ���ʼ������
    }

    RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits, 60);// 1 min
    return 0;
}

