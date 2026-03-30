#include <stdio.h>
#include "uart.h"
#include "i2c.h"
#include "lcd.h"
#include "systick.h"
#include "gpio.h"
#include "adc.h"

int sensor_value;

int main(void)
{
    uart_init();
    i2c1_init();
    lcd_init();

    lcd_send_string("Booting...");
    systick_msec_delay(1000);

    /*Initialize ADC*/
    pa1_adc_init();
    start_conversion();
    led_init();

    while(1)
    {
        sensor_value = adc_read();

        /* Update LCD */
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_send_string("ADC: ");
        lcd_print_number(sensor_value);

        lcd_set_cursor(1, 0);

        if(sensor_value > 2500)
        {
            lcd_send_string("Dark - LED ON");
            uart_print("Dark - LED ON\r\n");
            led_on();
        }
        else
        {
            lcd_send_string("Bright - LED OFF");
            uart_print("Bright - LED OFF\r\n");
            led_off();
        }

        systick_msec_delay(500);
    }
}
