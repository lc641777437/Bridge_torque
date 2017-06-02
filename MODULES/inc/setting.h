#ifndef __SETTING_H__
#define __SETTING_H__
#include "sys.h"
typedef enum{
    SEND_NULL = 0,
    SEND_CT   = 1,
    SEND_DT   = 2
} SEND_TYPE;

typedef enum{
    SEND_BY_NULL = 0,
    SEND_BY_UART1= 1,
    SEND_BY_UART3= 2
} SEND_BY_WHO;

void set_Save_Flag(u8 flag);
u8 get_Save_Flag(void);

void set_Send_Flag(SEND_BY_WHO who);
SEND_BY_WHO get_Send_Flag(void);

u8 get_isDynamic(void);
void set_isDynamic(u8 state);
void set_SendingType(SEND_TYPE type);
u8 get_isSendingType(SEND_TYPE type);

int get_DevID(void);
void set_DevID(int id);

int get_sampleTime(void);
void set_Frequent(int fre);

int get_SendTimeDynamic(void);
void set_SendTimeDynamic(int time);

void set_SendTimeServer(int time);
int get_SendTimeServer(void);

void set_CtrlState(u32 CtrlState);

long get_timestamp(void);
void add_timestamp(void);
void set_timestamp(long NewTimestamp);

void add_timestampDynamic(void);
long get_timestampDynamic(void);
void reset_timestampDynamic(void);

#define TIME_FOR_SAVE (60)
#define COUNT_10US_PER_S (100000)
#define ZERO_OF_21_CENTURE (946656000)
#define END_OF_21_CENTURE (4102416000)

#endif/*__SETTING_H__*/
