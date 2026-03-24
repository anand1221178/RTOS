#include <stdio.h>
#include "uart.h"
#include "spi.h"
#include "gpio.h"

/*SPI Loopback Test: wire PA7 (MOSI) -> PA6 (MISO)*/

#define TEST_SIZE 5
uint8_t tx_buf[TEST_SIZE] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
uint8_t rx_buf[TEST_SIZE];

int main(void)
{
    uart_init();
    spi_gpio_init();
    spi1_config();
    led_init();

    uart_print("=== SPI Loopback Test ===\r\n");

    /*Enable chip select (even though no slave, keeps the habit)*/
    cs_enable();

    /*Transfer data - send tx_buf while simultaneously receiving into rx_buf*/
    spi1_transfer(tx_buf, rx_buf, TEST_SIZE);

    cs_disable();

    cs_disable();

    /*Print results*/
    uart_print("TX: ");
    for(int i = 0; i < TEST_SIZE; i++)
    {
        uart_print_number(tx_buf[i]);
        uart_print(" ");
    }
    uart_print("\r\nRX: ");
    for(int i = 0; i < TEST_SIZE; i++)
    {
        uart_print_number(rx_buf[i]);
        uart_print(" ");
    }

    /*Check if loopback matched*/
    uint8_t pass = 1;
    for(int i = 0; i < TEST_SIZE; i++)
    {
        if(tx_buf[i] != rx_buf[i])
        {
            pass = 0;
            break;
        }
    }

    if(pass)
    {
        uart_print("\r\n>> PASS: Loopback matched!\r\n");
        led_on();
    }
    else
    {
        uart_print("\r\n>> FAIL: Mismatch detected\r\n");
        led_off();
    }

    while(1){}
}