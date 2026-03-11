#ifndef GPIO_H
#define GPIO_H

#include "stdbool.h"
#include "stm32f401xe.h"

void led_init(void);
void led_on(void);
void led_off(void);
void button_init(void);
bool get_btn_state(void);


#endif