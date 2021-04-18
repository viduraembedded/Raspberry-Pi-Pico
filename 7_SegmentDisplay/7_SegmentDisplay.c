#include "pico/stdio.h"
#include "hardware/gpio.h"
#include "pico/time.h"



#define BUTTON_PIN  22
#define SEG_A   0

uint8_t segmentNumber[10] = {
        0x3f,  // 0
        0x06,  // 1
        0x5b,  // 2
        0x4f,  // 3
        0x66,  // 4
        0x6d,  // 5
        0x7d,  // 6
        0x07,  // 7
        0x7f,  // 8
        0x67   // 9
};

uint8_t number = 0;

int main()
{
    // configure the GPIO
    // Initialize the GPIOs here
    gpio_init(BUTTON_PIN);

    // set direction of two gpios
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    // Enable the internal pullup resistor to Button
    gpio_pull_up(BUTTON_PIN);
    
    // Initializze the all segment gpios
    for (uint8_t i = SEG_A; i < SEG_A + 7 ; i++)
    {
        /* code */
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
    }
    

    while(1)
    {
        if (gpio_get(BUTTON_PIN) == false){
            if(number == 9){
                number = 0;
            }else{
                number++;
            }
        }
        else if(number == 0){
            number = 9;
        }
        else{
            number--;
        }

        // output the number to the seven segent dispaly

        uint8_t output = segmentNumber[number];
        gpio_put_all(output);
        sleep_ms(1000);
    }

    return 0;
}