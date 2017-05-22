/*
 * initstate.c
 *
 *  Created on: 2017/05/18
 *      Author: lc
 */
#include "initstate.h"

DEVICE_STATE sd_card = OFF;
DEVICE_STATE eth_host = OFF;
DEVICE_STATE usb_face = OFF;
DEVICE_STATE sd_fatfs_sd = OFF;
DEVICE_STATE sd_fatfs_usb = OFF;

void set_DeviceState(DEVICE device)
{
    if(device == DEVICE_SD)
    {
        sd_card = ON;
    }
    else if(device == DEVICE_USB)
    {
        usb_face = ON;
    }
    else if(device == DEVICE_FATFS_SD)
    {
        usb_face = ON;
    }
    else if(device == DEVICE_FATFS_USB)
    {
        sd_fatfs_usb = ON;
    }
}

void reset_DeviceState(DEVICE device)
{
    if(device == DEVICE_SD)
    {
        sd_card = OFF;
    }
    else if(device == DEVICE_USB)
    {
        usb_face = OFF;
    }
    else if(device == DEVICE_FATFS_SD)
    {
        usb_face = OFF;
    }
    else if(device == DEVICE_FATFS_USB)
    {
        sd_fatfs_usb = OFF;
    }
}

DEVICE_STATE get_DeviceState(DEVICE device)
{
    DEVICE_STATE state = OFF;

    if(device == DEVICE_SD)
    {
        state = sd_card;
    }
    else if(device == DEVICE_USB)
    {
        state = usb_face;
    }
    else if(device == DEVICE_FATFS_SD)
    {
        state = sd_fatfs_sd;
    }
    else if(device == DEVICE_FATFS_USB)
    {
        state = sd_fatfs_usb;
    }

    return state;
}

