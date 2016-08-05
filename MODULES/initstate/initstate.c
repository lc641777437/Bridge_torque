#include "initstate.h"

char sd_Init_State;
char eth_Init_State;
//1£ºsd¿¨   2£ºÍø¿Ú
void add_InitState(char mode)
{
    if(mode==1)
    {
        sd_Init_State++;
    }
    else if(mode==2)
    {
        eth_Init_State++;
    }
}

void reset_InitState(char mode)
{
    if(mode==1)
    {
        sd_Init_State=0;
    }
    else if(mode==2)
    {
        eth_Init_State=0;
    }
}

char get_InitState(char mode)
{
    if(mode==1)
    {
        return sd_Init_State;
    }
    else if(mode==2)
    {
        return eth_Init_State;
    }
    return 0;
}
