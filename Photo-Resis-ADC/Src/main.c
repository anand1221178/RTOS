#include <stdio.h>
#include "adc.h"
#include "uart.h"
#include "gpio.h"
int sensor_value;
int main(void)
{
    /*Initialize debug UART*/
    uart_init();
    /*Initialize ADC*/
    pa1_adc_init();
    /*Start conversion*/
    start_conversion();
    led_init();
    while(1)                                                 
                                            
    {                                                                                                
        sensor_value = adc_read();                                                                   
        uart_print("Brightness auto LED");                                                                        
        uart_print("ADC Value: ");                                                                  
        uart_print_number(sensor_value);                                                             
        uart_print(" - ");                                                                           

        if(sensor_value > 2500) {           // HIGH value = DARK                                     
            uart_print("Dark - Turning on LED\r\n");
            led_on();                                                              
        } else {                             // LOW value = BRIGHT                                   
            uart_print("Bright - turning off\r\n");     
            led_off();                                                           
        }                                                                                            

        for(volatile uint32_t i = 0; i < 2000000; i++);                                              
    }
}