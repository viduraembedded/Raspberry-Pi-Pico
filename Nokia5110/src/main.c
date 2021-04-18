#include<stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "main.h"
#include "Nokia5110.h"

int main()
{

    //intialize stdio
    stdio_init_all();

    // intialize the SPI0 of Raspberry Pi
        // This example will use SPI0 at 4MHz.
    spi_init(SPI_PORT, 4000 * 1000);
    //gpio_set_function(LCD_MISO, GPIO_FUNC_SPI);
    gpio_set_function(LCD_SCK, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(LCD_RST);
    gpio_set_dir(LCD_RST, GPIO_OUT);
    gpio_init(LCD_CS);
    gpio_set_dir(LCD_CS, GPIO_OUT);
    gpio_init(LCD_DC);
    gpio_set_dir(LCD_DC, GPIO_OUT);

    // call the LCD initialization
    Nokia5110_Init();
    clearDisplay();
    LCD_Demo();
    while(1)
    {

    }

    return 0;
}