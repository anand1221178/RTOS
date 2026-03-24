#include "gpio.h"

#define GPIOAEN (1U<<0) /*Enable clock access to GPIOA*/
#define GPIOCEN (1U<<2) /*Enable clock access to GPIOC*/
#define BTN_PIN (1U<<13) /*PC13 btn*/
#define LED_BS8 (1U<<8) /*Bit set for PA8*/
#define LED_BR8 (1U<<24) /*Bit reset for PA8*/
#define LED_PIN (1U<<8) /*PA8*/

void led_init(void)
{
    /*Enable clock access to GPIOA*/
    RCC->AHB1ENR |= GPIOAEN;

    /*Set PA5 mode to output mode*/
    GPIOA->MODER |=(1U<<16);
    GPIOA->MODER &=~(1U<<17);
}

void led_on(void)
{
    /*Set PA6 high*/
    GPIOA->BSRR =LED_BS8;
}

void led_off(void)
{
    /*Set PA6 high*/
    GPIOA->BSRR =LED_BR8;
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

void led_toggle(void)
{
    //Bit mask odr bit and see its its 1 then its on -> so turn off
    if (GPIOA->ODR & LED_PIN){
        led_off();
    }
    else{
        led_on();
    }
}