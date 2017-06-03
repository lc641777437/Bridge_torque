#include "gpio.h"
#include "setting.h"

static u8 save_Flag = 0;
static SEND_BY_WHO send_Flag = SEND_BY_NULL;

static u8 isDynamic = 0;
static SEND_TYPE SendingType = SEND_NULL;

static int devid = 1001;
static int send_time_dynamic = 1;// 1_s 1s
static int send_time_server = 1;// 1_min 1min
static int sample_time = 5 * 100; // 10_us 5ms

static long timestamp = ZERO_OF_21_CENTURE;
static int timestampDynamic = 0;

void set_Save_Flag(u8 flag)
{
    save_Flag = flag;
}

u8 get_Save_Flag(void)
{
    return save_Flag;
}

void set_Send_Flag(SEND_BY_WHO who)
{
    send_Flag = who;
}

SEND_BY_WHO get_Send_Flag(void)
{
    return send_Flag;
}

void set_isDynamic(u8 state)
{
    isDynamic = state;
    reset_timestampDynamic();
}

u8 get_isDynamic(void)
{
    return isDynamic;
}

void set_SendingType(SEND_TYPE type)
{
    SendingType = type;
}

u8 get_isSendingType(SEND_TYPE type)
{
    if(SendingType == type)return 1;
    else return 0;
}


void set_DevID(int id)
{
    devid = id;
}

int get_DevID(void)
{
    return devid;
}


void set_Frequent(int fre)
{
    sample_time = COUNT_10US_PER_S / fre;
}

int get_sampleTime(void)
{
    return sample_time;
}


void set_SendTimeDynamic(int time)
{
    send_time_dynamic = time;
}

int get_SendTimeDynamic(void)
{
    return send_time_dynamic;
}


void set_SendTimeServer(int time)
{
    send_time_server = time;
}

int get_SendTimeServer(void)
{
    return send_time_server;
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


void add_timestampDynamic(void)
{
    timestampDynamic++;
}

long get_timestampDynamic(void)
{
    return timestampDynamic;
}

void reset_timestampDynamic(void)
{
    timestampDynamic = 0;
}

