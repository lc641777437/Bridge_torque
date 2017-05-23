#ifndef __SETTING_H__
#define __SETTING_H__

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

#define TIME_FOR_SAVE (60)
#define US_COUNT_PER_S (100000)
#define ZERO_OF_21_CENTURE (946656000)
#define END_OF_21_CNTURE (4102416000)

#endif/*__SETTING_H__*/
