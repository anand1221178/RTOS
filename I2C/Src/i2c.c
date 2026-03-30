#include "stm32f4xx.h"

#define GPIOBEN (1U<<1)
#define I2C1EN (1U<<21)

#define I2C_100KHZ 80
#define SD_MODE_MAX_RISE_TIME 17

#define CR1_PE (1U<<0)
#define SR2_BUSY (1U<<1)
#define CR1_START (1U<<8)
#define SR1_SB (1U<<0)
#define SR1_ADDR (1U<<1)
#define SR1_TXE (1U<<7)
#define CR1_ACK (1U<<10)
#define CR1_STOP (1U<<9)

#define SR1_RXNE (1U<<6)
#define SR1_BTF (1U<<2)

/*Mapping:
PB8 -> SCL
PB9 -> SDA
*/

void i2c1_init(void)
{
    /*Enable clock access to GPIOB*/
    RCC->AHB1ENR |= GPIOBEN;

    /*Set PB8 and PB9 to alternate function mode*/
    //PB8
    GPIOB->MODER &=~(1U<<16);
    GPIOB->MODER |= (1U<<17);

    //PB9
    GPIOB->MODER &=~(1U<<18);
    GPIOB->MODER |= (1U<<19);

    /*SET PB8 and PB9 to otype to open drain*/
    GPIOB->OTYPER |= (1U<<8);
    GPIOB->OTYPER |= (1U<<9);

    /* Enable pull up for PB8 and PB9*/
    GPIOB->PUPDR |= (1U<<16);
    GPIOB->PUPDR &=~(1U<<17);

    GPIOB->PUPDR |= (1U<<18);
    GPIOB->PUPDR &=~(1U<<19);

    /*Set PB8 and PB9 to alternate function 4 since that is I2C SDA and SDL*/
    //PB8 - SCL
    GPIOB->AFR[1] &=~(1U<<3);
    GPIOB->AFR[1] |= (1U<<2);
    GPIOB->AFR[1] &=~(1U<<1);
    GPIOB->AFR[1] &=~(1U<<0);

    //PB9 -SDA
    GPIOB->AFR[1] &=~(1U<<7);
    GPIOB->AFR[1] |= (1U<<6);
    GPIOB->AFR[1] &=~(1U<<5);
    GPIOB->AFR[1] &=~(1U<<4);

    /*Enable clock access to I2C*/
    RCC->APB1ENR |= I2C1EN;


    //Enter REST MODE ON I2C
    I2C1->CR1 |= (1U<<15);

    //Exit rest mode:
    I2C1->CR1 &=~ (1U<<15);
    
    //Set the peripheral clock freq
    I2C1->CR2 = (1U<<4); //16 in binary basically telling peripheral that apb1 runs at 16mhz

    /*Set I2C to standard mode, 100kHz clock */

    //Weird ass math to follow:
    // This is the actual divider that determines how fast SCL toggles:
    //SCL Frequency = APB1_clock / (2 x CCR)
    //SCL_freq = 16 000 000 / (2xCCR)
    //Set SCL to 100 000
    // Hence 100 000 = 16 000 000 / (2 x 80)
    //100 000 = 100 000 -> So we are in standard mode
    I2C1->CCR = I2C_100KHZ;

    /*Set rise time */
    /*Another confusing constraint!:
    THis one is a physical constraint
    When SCL or SDL transitions from LOW to HIGH, the volatge doesnt rise instantly - it is gradual since the I2C bus uses open-drain with pull-resistors
    Standard mode (100khz) = 1000ns = 1us
    fast mode (400khz) = 300ns*
    clock_period = 1/ frequency
    = 1/ 16 000 000
    = 62.5 ns
    hence:
    TRISE  = 1000ns / 62.5ns + 1
    = 16+ 1 = 17/
    I2C1->TRISE = SD_MODE_MAX_RISE_TIME;

    /*Enable I2C1 module */
    I2C1->CR1 |= CR1_PE;
}



/*START → [0x4E] → ACK → [0x00] → ACK → RESTART → [0x4F] → ACK → [DATA] → NACK → STOP
        addr+W          reg addr        addr+R              slave sends
        
|______ "write" phase ______|         |_______ "read" phase ________|
*/

void i2c1_byte_read(char saddr, char maddr, char* data)
{
    volatile int temp;

    /*Wait until bus not busy*/
    while(I2C1->SR2 & (SR2_BUSY)){};

    /*Generate start*/
    I2C1->CR1 |= CR1_START;

    /*Wati until start flag is set*/
    while(!(I2C1->SR1 & (SR1_SB))){};

    /*Transmit slave addr + write bit
    the << 1 makes room for the r/w bit, Bit 0 = 0 means write, the slave sees its addres and responds with an ACK
    THIS IS ONLY FOR THE SLAVE ADDRESS THIS IS CONVENTION!*/
    I2C1->DR = saddr << 1;

    /* Wait until addr flag is set */
    while (!(I2C1->SR1 & (SR1_ADDR))){}

    /* Clear addr flag */
    temp = I2C1->SR2;

    /*send memory address BUT NO WRITE! we are wanting to access this reg address from the sensor or whatever we are connected to*/
    I2C1->DR = maddr;

    /*Wait until transmitter empty wait for TXE - the byte has been sent*/
    while (!(I2C1->SR1 & SR1_TXE)){}

    /*Generate restart - We restart instead of stop start because a STOP wpuld release the bus, and another master could steal it, the restart keeps us in control*/
    I2C1->CR1 |= CR1_START;

    /*IN THIS PHASE WE ARE NOW RECIEVEING THE DATA*/

    /* Wait until start flag is set */
    while (!(I2C1->SR1 & SR1_SB)){}

    /* Transmit slave address + Read */
    I2C1->DR = saddr << 1 | 1;

    /* Wait until addr flag is set */
    while (!(I2C1->SR1 & (SR1_ADDR))){}

    /* Disable Acknowledge */
    I2C1->CR1 &= ~CR1_ACK;

    /* Clear addr flag */
    temp = I2C1->SR2;

    /* Generate stop after data received */
    I2C1->CR1 |= CR1_STOP;

    /* Wait until RXNE flag is set */
    while (!(I2C1->SR1 & SR1_RXNE)){}

    /* Read data from DR */
    *data++ = I2C1->DR;

}


/*Function to read mulitple bytes from the slave device*/
void i2c1_burst_read(char saddr, char maddr, int n, char* data)
{
    volatile int tmp;

    /*Wait until bus is not busy*/
    while (I2C1->SR2 & (SR2_BUSY)) {};

    /*Generate start*/
    I2C1->CR1 |= CR1_START;

    /*Wait until start flag is set*/
    while(!(I2C1->SR1 & SR1_SB)){};

    /*Transmit the slave addr + write bit*/
    I2C1->DR = saddr << 1;

    /*Wait until slave addr is set*/
    while(!(I2C1->SR1 & SR1_ADDR)){};

    /*Clear sddr flag*/
    tmp = I2C1->SR2;

    /*Wait until transmitter empty*/
    while(!(I2C1->SR1 & SR1_TXE)){};

    /*Send memory addr*/
    I2C1->DR = maddr;

    /*Wait until transmitter empty*/
    while(!(I2C1->SR1 & SR1_TXE)){};

    /*generate restart*/
    I2C1->CR1 |= CR1_START;

    /*wait until start flag is set*/
    while(!(I2C1->SR1 & SR1_SB)) {};

    /*transmist slave address + read*/
    I2C1->DR = saddr << 1 |1;

    /* Wait until addr flag is set */
    while (!(I2C1->SR1 & (SR1_ADDR))){};

    /* Clear addr flag */
    tmp = I2C1->SR2;

    /* Enable Acknowledge */
    I2C1->CR1 |= CR1_ACK;


    while(n > 0U)
    {
        /*if one byte*/
        if(n == 1U)
        {
        /* Disable Acknowledge */
        I2C1->CR1 &= ~CR1_ACK;
        /* Generate Stop */
        I2C1->CR1 |= CR1_STOP;
        /* Wait for RXNE flag set */
        while (!(I2C1->SR1 & SR1_RXNE)){}
        /* Read data from DR */
        *data++ = I2C1->DR;
        break;
        }
        else
        {
            /* Wait until RXNE flag is set */
            while (!(I2C1->SR1 & SR1_RXNE)){}
            /* Read data from DR */
            (*data++) = I2C1->DR;
            n--;
        }
    }
}


void i2c1_burst_write(char saddr, char maddr, int n, char* data) {
    volatile int tmp;

    /* Wait until bus not busy */
    while (I2C1->SR2 & (SR2_BUSY)){}

    /* Generate start */
    I2C1->CR1 |= CR1_START;

    /* Wait until start flag is set */
    while (!(I2C1->SR1 & (SR1_SB))){}

    /* Transmit slave address */
    I2C1->DR = saddr << 1;

    /* Wait until addr flag is set */
    while (!(I2C1->SR1 & (SR1_ADDR))){}

    /* Clear addr flag */
    tmp = I2C1->SR2;

    /* Wait until data register empty */
    while (!(I2C1->SR1 & (SR1_TXE))){}

    /* Send memory address */
    I2C1->DR = maddr;

    for (int i = 0; i < n; i++) {
        /* Wait until data register empty */
        while (!(I2C1->SR1 & (SR1_TXE))){}
        /* Transmit memory address */
        I2C1->DR = *data++;
    }

    /* Wait until transfer finished */
    while (!(I2C1->SR1 & (SR1_BTF))){}
    
    /* Generate stop */
    I2C1->CR1 |= CR1_STOP;
}


/*Simple write with no register address — used for devices like the PCF8574
  that don't have internal registers, just raw pin state*/
void i2c1_write_no_reg(char saddr, char data)
{
    volatile int tmp;

    /* Wait until bus not busy */
    while (I2C1->SR2 & (SR2_BUSY)){}

    /* Generate start */
    I2C1->CR1 |= CR1_START;

    /* Wait until start flag is set */
    while (!(I2C1->SR1 & (SR1_SB))){}

    /* Transmit slave address + write */
    I2C1->DR = saddr << 1;

    /* Wait until addr flag is set */
    while (!(I2C1->SR1 & (SR1_ADDR))){}

    /* Clear addr flag */
    tmp = I2C1->SR2;

    /* Wait until data register empty */
    while (!(I2C1->SR1 & (SR1_TXE))){}

    /* Send the data byte */
    I2C1->DR = data;

    /* Wait until transfer finished */
    while (!(I2C1->SR1 & (SR1_BTF))){}

    /* Generate stop */
    I2C1->CR1 |= CR1_STOP;
}