#include <stdio.h>
#include "uart.h"

int main(void)
{
    /*Initialize debug UART*/
    uart_init();
    while(1)
    {
    // TODO(human): Replace this printf with a simple direct UART test
    // Try calling __io_putchar('A') or sending characters one by one
    uart_print("Hello from AUROM rtos\r\n");

    // Simple delay (counts to ~1 million for visibility)         
          for(volatile uint32_t i = 0; i < 1000000; i++);  
    }
}