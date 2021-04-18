#include "pico/stdio.h"
#include "hardware/gpio.h"
#include "pico/time.h"


#define LED_PIN     25
#define BUTTON_PIN  22
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
    while(1)
    {
        if ( gpio_get(BUTTON_PIN) == false)
        {
            // wait for 100ms to reduce the debounce effect
            sleep_ms(100);
            if (gpio_get(BUTTON_PIN) == false)
            {
                gpio_put(LED_PIN, 1);
            }
        }
        else{
            gpio_put(LED_PIN, 0);
        }
    }

    return 0;
}