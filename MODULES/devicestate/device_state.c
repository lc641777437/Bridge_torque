#include "device_state.h"

DEVICE_STATE sd_card = OFF;
DEVICE_STATE eth_host = OFF;
DEVICE_STATE usb_face = OFF;
DEVICE_STATE sd_fatfs = OFF;

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
    else if(device == DEVICE_FATFS)
    {
        usb_face = ON;
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
    else if(device == DEVICE_FATFS)
    {
        usb_face = OFF;
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
    else if(device == DEVICE_FATFS)
    {
        state = sd_fatfs;
    }

    return state;
}
