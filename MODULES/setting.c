#include "setting.h"

#define US_COUNT_PER_S (100000)

static int sample_time = 5 * 100;

void set_Frequent(int fre)
{
    sample_time = US_COUNT_PER_S / fre;
}

int get_sampleTime(void)
{
    return sample_time;
}

