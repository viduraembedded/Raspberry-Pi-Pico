#include<stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

int main()
{

    //intialize stdio
    stdio_init_all();
    //intialize the HW ADC
    adc_init();
    //enable adc gpio pin
    adc_gpio_init(26);
    adc_gpio_init(27);
    adc_gpio_init(28);
    //enable select adc channel that we want read
    adc_select_input(0);
    adc_select_input(1);
    adc_select_input(2);

    while(1)
    {
        int16_t Z_out = adc_read(); // reading the adc raw value and assign it to adcValue variable
        int16_t Y_out = adc_read();
        int16_t X_out = adc_read();
        printf(" X_out = %d  Y_out = %d  Z_out = %d \r\n ", X_out, Y_out,Z_out);
        sleep_ms(500);
    }
    return 0;
}