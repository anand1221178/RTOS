#include "gpio.h"

bool btn_state;


int main(void)
{
    /*Initialize LED*/
    led_init();
    
    //intialise button:
    button_init();


    while(1)
    {
        //Getbutton state:
        btn_state = get_btn_state();

        if(btn_state){
            led_on();
        }
        else{
            led_off();
        }
        
    }
}