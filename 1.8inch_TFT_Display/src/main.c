#include<stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "main.h"
#include "st7735.h"
#include "fonts.h"
#include "testimg.h"

void DemoTFT(void);
uint8_t r=0;

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
    gpio_init(LCD_DC); //RS PIn
    gpio_set_dir(LCD_DC, GPIO_OUT);

    // call the LCD initialization
    ST7735_Init();


    while(1)
    {
        DemoTFT();
    }

    return 0;
}


void DemoTFT(void)
{
	ST7735_SetRotation(r);

	ST7735_FillScreen(ST7735_BLACK);

	for(int x = 0; x < ST7735_GetWidth(); x++)
	{
	  ST7735_DrawPixel(x, 0, ST7735_WHITE);
	  ST7735_DrawPixel(x, ST7735_GetHeight() - 1, ST7735_WHITE);
	}

	for(int y = 0; y < ST7735_GetHeight(); y++)
	{
	  ST7735_DrawPixel(0, y, ST7735_WHITE);
	  ST7735_DrawPixel(ST7735_GetWidth() - 1, y, ST7735_WHITE);
	}

	ST7735_DrawLine(0, 0, ST7735_GetWidth(), ST7735_GetHeight(), ST7735_WHITE);
	ST7735_DrawLine(ST7735_GetWidth(), 0, 0, ST7735_GetHeight(), ST7735_WHITE);

	sleep_ms(2000);

	ST7735_FillScreen(ST7735_BLACK);

	for (int i = 0; i < ST7735_GetHeight(); i += 4)
	{
		ST7735_DrawFastHLine(0, i, ST7735_GetWidth() - 1, ST7735_WHITE);
	}

	for (int i = 0; i < ST7735_GetWidth(); i += 4)
	{
		ST7735_DrawFastVLine(i, 0, ST7735_GetHeight() - 1, ST7735_WHITE);
	}

	sleep_ms(2000);

	// Check fonts
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_DrawString(0, 0, "Font_7x10, red on black, lorem ipsum dolor sit amet", Font_7x10, ST7735_RED, ST7735_BLACK);
	ST7735_DrawString(0, 3*10, "Font_11x18, green, lorem ipsum", Font_11x18, ST7735_GREEN, ST7735_BLACK);
	ST7735_DrawString(0, 3*10+3*18, "Font_16x26", Font_16x26, ST7735_BLUE, ST7735_BLACK);
	sleep_ms(2000);

	// Check colors
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_DrawString(0, 0, "BLACK", Font_11x18, ST7735_WHITE, ST7735_BLACK);
	sleep_ms(500);

	ST7735_FillScreen(ST7735_BLUE);
	ST7735_DrawString(0, 0, "BLUE", Font_11x18, ST7735_BLACK, ST7735_BLUE);
	sleep_ms(500);

	ST7735_FillScreen(ST7735_RED);
	ST7735_DrawString(0, 0, "RED", Font_11x18, ST7735_BLACK, ST7735_RED);
	sleep_ms(500);

	ST7735_FillScreen(ST7735_GREEN);
	ST7735_DrawString(0, 0, "GREEN", Font_11x18, ST7735_BLACK, ST7735_GREEN);
	sleep_ms(500);

	ST7735_FillScreen(ST7735_CYAN);
	ST7735_DrawString(0, 0, "CYAN", Font_11x18, ST7735_BLACK, ST7735_CYAN);
	sleep_ms(500);

	ST7735_FillScreen(ST7735_MAGENTA);
	ST7735_DrawString(0, 0, "MAGENTA", Font_11x18, ST7735_BLACK, ST7735_MAGENTA);
	sleep_ms(500);

	ST7735_FillScreen(ST7735_YELLOW);
	ST7735_DrawString(0, 0, "YELLOW", Font_11x18, ST7735_BLACK, ST7735_YELLOW);
	sleep_ms(500);

	ST7735_FillScreen(ST7735_WHITE);
	ST7735_DrawString(0, 0, "WHITE", Font_11x18, ST7735_BLACK, ST7735_WHITE);
	sleep_ms(500);

	// Draw circles
	ST7735_FillScreen(ST7735_BLACK);
	for (int i = 0; i < ST7735_GetHeight() / 2; i += 2)
	{
		ST7735_DrawCircle(ST7735_GetWidth() / 2, ST7735_GetHeight() / 2, i, ST7735_YELLOW);
	}
	sleep_ms(1000);

	ST7735_FillScreen(ST7735_BLACK);
	ST7735_FillTriangle(0, 0, ST7735_GetWidth() / 2, ST7735_GetHeight(), ST7735_GetWidth(), 0, ST7735_RED);
	sleep_ms(1000);

	ST7735_FillScreen(ST7735_BLACK);
	ST7735_DrawImage(10, 30, 100, 100, (uint16_t*) test_img);
	sleep_ms(3000);

	r++;
}