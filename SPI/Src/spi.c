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

/*Takes a pointer to a byte array and how many bytes to send*/
void spi1_transmit(uint8_t *data, uint32_t size)
{
    uint32_t i=0;
    uint32_t temp;

    while(i<size)
    {
        /*wait until TXE is set*/
        /*SR_TXE will tell us if the internal transmit buffere is ready to accept new data.*/
        while(!(SPI1->SR & (SR_TXE))){} // IF txe is 0 -> buffer is full -> not ready

            /*Write the byte to the data register -> over here we work byte by byte*/
            SPI1->DR = data[i];
            i++;
    }

    
    while(!(SPI1->SR & (SR_TXE))){}; //wait for last byte to leve TX buffer

    /*Wait for BUSY flag to reset*/
    while((SPI1->SR & (SR_BSY))){};

    /*Clear OVR flag*/
    temp = SPI1->DR;
    temp = SPI1->SR;
}

void spi1_receive(uint8_t *data, uint32_t size)
{
    /*Masetr always generates the clock and the slave can only send data back while the clock is running.
    but clock only runs when the master transmits something
    so to recieve data the master must transmit dummy data to keep the clock ticking*/
    while(size)//loop until all bytes recieved
    {
        /*Send dummy data*/
        SPI1->DR =0;
        /*Wait for RXNE flag to be set*/
        while(!(SPI1->SR & (SR_RXNE))){};
        /*Read data from data register*/
        *data++ = (SPI1->DR);
        size--;
    }
}

/*Full-duplex SPI transfer: sends tx_data while receiving into rx_data*/
void spi1_transfer(uint8_t *tx_data, uint8_t *rx_data, uint32_t size)
{
    /* TODO(human): Implement the transfer loop
     * For each byte you need to:
     * 1. Wait for TXE (transmit buffer empty)
     * 2. Write tx_data[i] to SPI1->DR
     * 3. Wait for RXNE (receive buffer not empty)
     * 4. Read SPI1->DR into rx_data[i]
     *
     * After the loop, wait for the last byte to finish:
     * - Wait for TXE
     * - Wait for BSY to clear
     * - Clear the OVR flag (read DR then SR)
     */
    uint32_t i =0;
    uint32_t temp;
    while(i<size){
        while(!(SPI1->SR & (SR_TXE))){}; //spin until buffer is ready
        SPI1->DR = tx_data[i]; //writing real data generates the clock — no dummy needed

        while(!(SPI1->SR & (SR_RXNE))){};  //wait for the simultaneous receive
        rx_data[i] = (SPI1->DR);
        i++;
    }

    while(!(SPI1->SR & (SR_TXE))){};
    while((SPI1->SR & (SR_BSY))){};

    /*Clear OVR flag*/
    temp = SPI1->DR;
    temp = SPI1->SR;
}

void cs_enable(void)
{
    GPIOA->ODR &=~(1U<<9);
}


/*Pull hgih to disavle*/
void cs_disable(void)
{
    GPIOA->ODR |=(1U<<9);
}