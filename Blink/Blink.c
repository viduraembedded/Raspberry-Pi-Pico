#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LED_PIN 25

int main()
{
    //1. Initialize the GPIO 25
    gpio_init(LED_PIN);
    //2. need to set the direction of the GPIO pin
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while(1)
    {
        gpio_put(LED_PIN, 1); // writting 1 to the GPIO PIN
        sleep_ms(250);
        gpio_put(LED_PIN, 0); // writting 0 to the GPIO PIN
        sleep_ms(250);
    }
    return 0;
}