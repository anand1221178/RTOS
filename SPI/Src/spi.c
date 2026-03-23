#include "spi.h"
#define SPIEN (1U<<12) //in SPI_CR
#define GPIOAEN (1u<<0) //general gpioa en
#define SR_TXE (1U<<1) //in SPI_SR
#define SR_RXNE (1U<<0) //in SPI_SR
#define SR_BSY (1U<<7) //in SPI_SR

/*Enable SPI func*/
void spi_gpio_init(void)
{
    //enable clock access to GPIOA
    RCC->AHB1ENR |= GPIOAEN;

    //Set PA5, 6 and 7 to alrenate function mode
    /*PA5*/
    GPIOA->MODER &=~(1U<<10);
    GPIOA->MODER |= (1U<<11);

    /*Pa6*/
    GPIOA->MODER &=~(1U<<12);
    GPIOA->MODER |= (1U<<13);

    /*PA7*/
    GPIOA->MODER &=~(1U<<14);
    GPIOA->MODER |= (1U<<15);

    //Set pa9 as output mode:
    GPIOA->MODER |= (1U<<18);
    GPIOA->MODER &=~ (1U<<19);

    //set pings 5 6 and 7 alternate fucntion mode to SPI1
    //PA5 gets SPI1_SCK -> AF05
    //PA6 gets SPI1_MISO -> AF05
    //PA7 gets SPI1_MOSI -> AF05

    /*PA5*/
    GPIOA->AFR[0] |=(1U<<20); //1
    GPIOA->AFR[0] &=~(1U<<21); //0
    GPIOA->AFR[0] |=(1U<<22); //1
    GPIOA->AFR[0] &=~(1U<<23); //0

    /*PA6*/
    GPIOA->AFR[0] |=(1U<<24);
    GPIOA->AFR[0] &= ~(1U<<25);
    GPIOA->AFR[0] |=(1U<<26);
    GPIOA->AFR[0] &= ~(1U<<27);


    /*PA7*/
    GPIOA->AFR[0] |=(1U<<28);
    GPIOA->AFR[0] &= ~(1U<<29);
    GPIOA->AFR[0] |=(1U<<30);
    GPIOA->AFR[0] &= ~(1U<<31);
}


void spi1_config(void)
{
    /*enable clock access to SPI1*/
    RCC->APB2ENR |= SPIEN;

    /*Set clock to FPCLK/4*/
    SPI1->CR1 |= (1U<<3);
    SPI1->CR1 &=~ (1U<<4);
    SPI1->CR1 &=~ (1U<<5);

    /*Set CPOL and CPHA to 1*/
    SPI1->CR1 |= (1U<<0);
    SPI1->CR1 |= (1U<<1);

    /*enable full duplex (send and rec on same line)*/
    SPI1->CR1 &=~(1U<<10);

    /*set MSB first*/
    SPI1->CR1 &=~(1U<<7);

    /*Set mode to MASTER*/
    SPI1->CR1 |= (1U<<2);

    /*Set 8 bit data mode*/
    SPI1->CR1 &= ~(1U<<11);

    /*Select software slave management by
    * setting SSM=1 and SSI=1*/
    SPI1->CR1 |= (1<<8);
    SPI1->CR1 |= (1<<9);

    /*Enable SPI module*/
    SPI1->CR1 |= (1<<6);
}