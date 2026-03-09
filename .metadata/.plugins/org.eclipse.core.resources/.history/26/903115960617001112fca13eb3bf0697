// Define base address for peripherals:
#define PERIPH_BASE (0x40000000UL)

//Offset for AHB1 bus:
#define AHB1_PERIPH_OFFSET (0x00020000UL)

//Define AHB1 address base:
#define AHB1PERIPH_BASE (PERIPH_BASE + AHB1_PERIPH_OFFSET)

//Define offset for GPIOA in AHB1 
#define GPIOA_OFFSET (0x0000UL)

//Define offset for GPIOC in AHB1
#define GPIOC_OFFSET (0x00000800UL)

//Base address for GPIOA and GPIOC
#define GPIOA_BASE (AHB1PERIPH_BASE + GPIOA_OFFSET)
#define GPIOC_BASE (AHB1PERIPH_BASE + GPIOC_OFFSET)

//Define RCC offset:
#define RCC_OFFSET (0x00003800UL)

//Define RCC base address:
#define RCC_BASE (AHB1PERIPH_BASE + RCC_OFFSET)

//Define offset of AHB1ENR reg
#define RCC_AHB1ENR_OFFSET (0x00000030UL)

//Address of AHB1ENR reg since we manipulate the bits here we need to use volatile
#define RCC_AHB1ENR (*(volatile unsigned int *)(RCC_BASE + RCC_AHB1ENR_OFFSET))

//Offset + address for mode reg:
#define MODER_OFFSET (0x00UL)
#define GPIOA_MODER (*(volatile unsigned int *)(GPIOA_BASE + MODER_OFFSET))
#define GPIOC_MODER (*(volatile unsigned int *)(GPIOC_BASE + MODER_OFFSET))

//Offset for output data register
#define ODR_OFFSET (0x14UL)

// Address of GPIOA output data register
#define GPIOA_ODR (*(volatile unsigned int *)(GPIOA_BASE + ODR_OFFSET))

//Offset for input data register
#define IDR_OFFSET (0x10UL)

//address of GPIOC IDR:
#define GPIOC_IDR (*(volatile unsigned int *)(GPIOC_BASE + IDR_OFFSET))

//bit mask for enabling GPIOA and GPIOC
#define GPIOAEN (1U<<0)
#define GPIOCEN (1U<<2)

//BIt mask for setting GPIOA pin 6 to output
#define PIN6 (1U<<6)

//BIT mask for setting GPIOC pin to IDR
#define PIN13 (1U<<13)

//16. Alias for PIN5 representing LED pin
#define LED_PIN PIN6
#define USER_BUTTON PIN13

//Start of main
int main(void){
  //First enable RCC to the GPIO ports
  RCC_AHB1ENR |= GPIOAEN;
  RCC_AHB1ENR |= GPIOCEN;

  //set the modes of each pin:
  //Set GPIOA pin 6 to GPOM:
  
  GPIOA_MODER &=~(1U<<13);//set bit 13 to 0
  GPIOA_MODER |=(1U<<12); //set bit 12 to 1

  //set GPIOC pin 13 bits 26 and 27 to input
  GPIOC_MODER  &=~(1U<<27);
  GPIOC_MODER &=~(1U <<26);

  while(1){
    //USer push button is a active low circuit!:
      //so if we are actually not pressing the button it is pushing 3.3 volts into pin 13 - IDR bit will read 1
      // When we press the button the volts are drained into the groud, so pin 13 will get no power so it will read 0
    // Read the IDR. Isolate bit 13. Is the result equal to 0?
    //Since we have set GPIOC to IDR, if it feels 3.3volts it will flip bit 13 to 1
    //If it feels 0 it will ground the power and force bit 13 to 0

    if ((GPIOC_IDR & USER_BUTTON) == 0) { //
        
        // The button is PRESSED! The pin is grounded.
        // Tell the ODR to supply voltage to the LED.
        GPIOA_ODR |= LED_PIN; //We are writing 1 to bit 6 -> ODR (OUTPUT DATA REG!)
        
    } else {
        
        // The button is UNPRESSED! The pin is seeing 3.3V.
        // Tell the ODR to shut off voltage to the LED.
        GPIOA_ODR &= ~LED_PIN;  //We are clearing the bit (setting to 0 since we dont want to supply with voltage)
        
    }
  }
}