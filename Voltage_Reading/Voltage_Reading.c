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
    //enable select adc channel that we want read
    adc_select_input(0);

    while(1)
    {
        uint16_t adcValue = adc_read(); // reading the adc raw value and assign it to adcValue variable
        float voltage = adcValue * (3.3/4095.0);
        printf(" ADC Raw Value = %d Voltage = %f \r\n",adcValue, voltage );
        sleep_ms(1000);
    }
    return 0;
}