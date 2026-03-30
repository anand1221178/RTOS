#ifndef __UART_H__
#define __USRT_H__

#include "stm32f4xx.h"

void uart_init(void);
int __io_putchar(int ch);
void uart_print(const char *str);
void uart_print_number(uint32_t num); 
#endif

