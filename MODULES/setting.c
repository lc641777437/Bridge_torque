#include "gpio.h"
#include "setting.h"

static int sample_time = 5 * 100;
static long timestamp = ZERO_OF_21_CENTURE;

void set_Frequent(int fre)
{
    sample_time = US_COUNT_PER_S / fre;
}

void set_CtrlState(u32 CtrlState)
{
    CTRL_0=(CtrlState&(1<<0))?1:0;
    CTRL_1=(CtrlState&(1<<1))?1:0;
    CTRL_2=(CtrlState&(1<<2))?1:0;
    CTRL_3=(CtrlState&(1<<3))?1:0;
    CTRL_4=(CtrlState&(1<<4))?1:0;
    CTRL_5=(CtrlState&(1<<5))?1:0;
    CTRL_6=(CtrlState&(1<<6))?1:0;
    CTRL_7=(CtrlState&(1<<7))?1:0;
    CTRL_8=(CtrlState&(1<<8))?1:0;
    CTRL_9=(CtrlState&(1<<9))?1:0;
    CTRL_10=(CtrlState&(1<<10))?1:0;
    CTRL_11=(CtrlState&(1<<11))?1:0;
    CTRL_12=(CtrlState&(1<<12))?1:0;
    CTRL_13=(CtrlState&(1<<13))?1:0;
    CTRL_14=(CtrlState&(1<<14))?1:0;
    CTRL_15=(CtrlState&(1<<15))?1:0;
}

int get_sampleTime(void)
{
    return sample_time;
}


void add_timestamp(void)
{
    timestamp++;
}

long get_timestamp(void)
{
    return timestamp;
}

void set_timestamp(long NewTimestamp)
{
    timestamp = NewTimestamp;
}

