#include "gpio.h"

#define GPIOAEN (1U<<0) /* Used to enable the clock for gpioa -> by writing to AHB1ENR reg*/
#define GPIOCEN (1U<<2) /*Enable vlock access to GPIOC*/
#define BTN_PIN (1U<<13) /*PC13 btn*/
#define LED_BS6 (1U<<6) /*Bit set opertion for pin 5 -> we BSSR PA5 to high -> turns on LED*/
#define LED_BR6 (1U<<22) /*Bit reset 5 -> set BSSR to low for pin 5 -> turns it off*/

void led_init(void)
{
    /*Enable clock access to GPIOA*/
    RCC->AHB1ENR |= GPIOAEN;

    /*Set PA5 mode to output mode*/
    GPIOA->MODER |=(1U<<12);
    GPIOA->MODER &=~(1U<<13);
}

void led_on(void)
{
    /*Set PA6 high*/
    GPIOA->BSRR =LED_BS6;
}

void led_off(void)
{
    /*Set PA6 high*/
    GPIOA->BSRR =LED_BR6;
}

void button_init(void){
    /*Enable clock access to PORT C*/
    RCC->AHB1ENR |= GPIOCEN;

    /*Set pin13 as input pin*/
    GPIOC->MODER &=~(1U<<26); //make 0
    GPIOC->MODER &=~(1U<<27); //make 0 
}

bool get_btn_state(void){
    //Since we have an active low cricuit when the value is 0 we are pressing

    if(GPIOC->IDR & BTN_PIN){
        return false;
    }
    else{
        return true;
    }
}