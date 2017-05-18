#ifndef __INITSTATE_H
#define __INITSTATE_H
#include "stm32f4xx.h"

typedef enum{
    DEVICE_NULL = 0,
    DEVICE_SD   = 1,
    DEVICE_ETH  = 2,
    DEVICE_USB  = 3,
    DEVICE_FATFS= 4,
} DEVICE;

typedef enum{
    OFF = 0,
    ON  = 1,
} DEVICE_STATE;

void set_DeviceState(DEVICE device);
void reset_DeviceState(DEVICE device);
DEVICE_STATE get_DeviceState(DEVICE device);

#endif


