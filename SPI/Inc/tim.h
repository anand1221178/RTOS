#include "stm32f4xx.h"

#ifndef TIM_H_
#define TIM_H_

#define SR_UIF (1U<<0) //UIF in SR of the timer,, when the timer overflows or reaches auot reload value this flag will be set, indicating a UEV

void tim2_1hz_init(void);

#endif