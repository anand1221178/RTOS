#include "systick.h"

#define CTRL_ENABLE (1U<<0)
#define CTRL_CLCKSRC (1U<<2)
#define CTRL_COUNTFLAG (1U<<16)

#define ONE_MSEC_LOAD 16000

void systick_msec_delay(uint32_t delay){


    //Load number of clck cycles for 1ms interrupt:
    SysTick->LOAD = ONE_MSEC_LOAD - 1;
    
    //clear systick current value
    SysTick->VAL = 0;

    //Select the internal clock source
    SysTick->CTRL = CTRL_CLCKSRC;

    //Enable systick
    SysTick->CTRL |= CTRL_ENABLE;

    for (int i = 0; i < delay; i ++){
        while (!(SysTick->CTRL & CTRL_COUNTFLAG)){}
    }

    //Disable it 
    SysTick->CTRL = 0;
}