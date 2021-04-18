#include "pico/stdio.h"
#include "hardware/gpio.h"
#include "pico/time.h"


#define LED_PIN     25
#define BUTTON_PIN  22

void GPIO_Button_Callabck(uint gpio, uint32_t events);
uint8_t modeSelection = 0;

int main()
{
    // configure the GPIO
    // Initialize the GPIOs here
    gpio_init(LED_PIN);
    gpio_init(BUTTON_PIN);

    // set direction of two gpios
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    // Enable the internal pullup resistor to Button
    gpio_pull_up(BUTTON_PIN);
    // Enable the interrupt for Button pin
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &GPIO_Button_Callabck);

    while(1)
    {
        if (modeSelection == 0)
        {
            /* code */
            // toggole LED with a delay of 250 ms
            gpio_put(LED_PIN, 1);
            sleep_ms(250);
            gpio_put(LED_PIN, 0);
            sleep_ms(250);
        }
        else if (modeSelection == 1)
        {
            /* code */
            //turn of the LED
            gpio_put(LED_PIN, 0);
            sleep_ms(250);
        }
        else if(modeSelection == 2)
        {
            /* code */
            gpio_put(LED_PIN, 1);
            sleep_ms(250);
        }
        
        
    }

    return 0;
}

void GPIO_Button_Callabck(uint gpio, uint32_t events)
{
    if (gpio == BUTTON_PIN){
        modeSelection++;
        if(modeSelection>2){
            modeSelection = 0;
        }
    }
}

