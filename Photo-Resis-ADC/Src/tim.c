#include "../Inc/tim.h"

#define TIM2EN (1u<<0)
#define CR1_CEN (1U<<0)

void tim2_1hz_init(void){
    /* 1. Enable clock access to TIM2 */
    RCC->APB1ENR |= TIM2EN;

    /* 2. Slow the 16MHz clock down to 1000 ticks per second */
    TIM2->PSC = 16000 - 1; 

    /* 3. Count 1000 of those ticks to create exactly a 1 second delay */
    TIM2->ARR = 1000-1 ; 

    /* 4. Clear the counter so it starts fresh at 0 */
    TIM2->CNT = 0;

    /* 5. Turn the timer on */
    TIM2->CR1 = CR1_CEN;
}