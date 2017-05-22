/*
 * initstate.h
 *
 *  Created on: 2017/05/18
 *      Author: lc
 */
#ifndef __INITSTATE_H__
#define __INITSTATE_H__
#include "stm32f4xx.h"

typedef enum{
    OFF = 0,
    ON  = 1,
} DEVICE_STATE;

typedef enum{
    DEVICE_NULL = 0,
    DEVICE_SD   = 1,
    DEVICE_ETH  = 2,
    DEVICE_USB  = 3,
    DEVICE_FATFS_SD = 4,
    DEVICE_FATFS_USB = 5,
} DEVICE;

void set_DeviceState(DEVICE device);
void reset_DeviceState(DEVICE device);
DEVICE_STATE get_DeviceState(DEVICE device);

#endif/*__INITSTATE_H__*/
