/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
 */

/* This is the original Adafruit Nokia5110 & GFX library
 * Nokia5110.c
 *
 *  Created and Modified on: 05-Oct-2018
 *      Author: Vidura Embedded
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "main.h"
#include "Nokia5110.h"

/// Font data stored PER GLYPH
void cp437(bool x);

typedef struct {
	uint16_t bitmapOffset;     ///< Pointer into GFXfont->bitmap
	uint8_t  width;            ///< Bitmap dimensions in pixels
	uint8_t  height;           ///< Bitmap dimensions in pixels
	uint8_t  xAdvance;         ///< Distance to advance cursor (x axis)
	int8_t   xOffset;          ///< X dist from cursor pos to UL corner
	int8_t   yOffset;          ///< Y dist from cursor pos to UL corner
} GFXglyph;

/// Data stored for FONT AS A WHOLE
typedef struct {
	uint8_t  *bitmap;      ///< Glyph bitmaps, concatenated
	GFXglyph *glyph;       ///< Glyph array
	uint8_t   first;       ///< ASCII extents (first char)
	uint8_t   last;        ///< ASCII extents (last char)
	uint8_t   yAdvance;    ///< Newline distance (y axis)
} GFXfont;

GFXfont *gfxFont = NULL;

void setFont(const GFXfont *f);

uint16_t _width = LCDWIDTH;
uint16_t _height = LCDHEIGHT;
uint16_t cursor_x = 0;
uint16_t cursor_y = 0;
uint16_t textcolor = 0xFFFF;
uint16_t textbgcolor = 0xFFFF;
uint8_t textsize = 1;
uint8_t rotation = 0;
bool wrap = true;
bool _cp437 = false;

#ifndef _BV
#define _BV(x) (1 << (x))
#endif


#define min(a,b) (((a) < (b)) ? (a) : (b))


#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif


#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

#ifndef pgm_read_word
	#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
	#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

// Pointers are a peculiar case...typically 16-bit on AVR boards,
// 32 bits elsewhere.  Try to accommodate both...

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

// the memory buffer for the LCD
uint8_t pcd8544_buffer[LCDWIDTH * LCDHEIGHT / 8] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFC, 0xFE, 0xFF, 0xFC, 0xE0,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8,
		0xF8, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80, 0xC0, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x7F,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x87, 0x8F, 0x9F, 0x9F, 0xFF, 0xFF, 0xFF,
		0xC1, 0xC0, 0xE0, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE,
		0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xE0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x80, 0xC0, 0xE0, 0xF1, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x1F, 0x0F, 0x0F, 0x87,
		0xE7, 0xFF, 0xFF, 0xFF, 0x1F, 0x1F, 0x3F, 0xF9, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xFD, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xF0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
		0x7E, 0x3F, 0x3F, 0x0F, 0x1F, 0xFF, 0xFF, 0xFF, 0xFC, 0xF0, 0xE0, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFC, 0xF0, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01,
		0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0F, 0x1F, 0x3F, 0x7F, 0x7F,
		0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x1F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


// reduces how much is refreshed, which speeds it up!
// originally derived from Steve Evans/JCW's mod but cleaned up and
// optimized
//#define enablePartialUpdate

#ifdef enablePartialUpdate
static uint8_t xUpdateMin, xUpdateMax, yUpdateMin, yUpdateMax;
#endif

static void updateBoundingBox(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax) {
#ifdef enablePartialUpdate
	if (xmin < xUpdateMin) xUpdateMin = xmin;
	if (xmax > xUpdateMax) xUpdateMax = xmax;
	if (ymin < yUpdateMin) yUpdateMin = ymin;
	if (ymax > yUpdateMax) yUpdateMax = ymax;
#endif
}

// the most basic function, set a single pixel
void drawPixel(int16_t x, int16_t y, uint16_t color) {
	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
		return;

	int16_t t;
	switch(rotation){
	case 1:
		t = x;
		x = y;
		y =  LCDHEIGHT - 1 - t;
		break;
	case 2:
		x = LCDWIDTH - 1 - x;
		y = LCDHEIGHT - 1 - y;
		break;
	case 3:
		t = x;
		x = LCDWIDTH - 1 - y;
		y = t;
		break;
	}

	if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
		return;

	// x is which column
	if (color)
		pcd8544_buffer[x+ (y/8)*LCDWIDTH] |= _BV(y%8);
	else
		pcd8544_buffer[x+ (y/8)*LCDWIDTH] &= ~_BV(y%8);

	updateBoundingBox(x,y,x,y);
}


// the most basic function, get a single pixel
uint8_t getPixel(int8_t x, int8_t y) {
	if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
		return 0;

	return (pcd8544_buffer[x+ (y/8)*LCDWIDTH] >> (y%8)) & 0x1;
}


void Nokia5110_Init(void) {

	uint8_t contrast = 63;
	uint8_t bias = 0x04;
	// Setup hardware SPI.
	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

	// toggle RST low to reset
	gpio_put(LCD_RST, 0);
	sleep_ms(200);
	gpio_put(LCD_RST, 1);

	// get into the EXTENDED mode!
	command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );

	// LCD bias select (4 is optimal?)
	command(PCD8544_SETBIAS | bias);

	// set VOP
	if (contrast > 0x7f)
		contrast = 0x7f;

	command( PCD8544_SETVOP | contrast); // Experimentally determined


	// normal mode
	command(PCD8544_FUNCTIONSET);

	// Set display to Normal
	command(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);

	// initial display line
	// set page address
	// set column address
	// write display data

	// set up a bounding box for screen updates

	updateBoundingBox(0, 0, LCDWIDTH-1, LCDHEIGHT-1);
	// Push out pcd8544_buffer to the Display (will show the AFI logo)
	display();
}

inline void spiWrite(uint8_t spiData) {

	// Hardware SPI write.
	//SPI.transfer(d);
	spi_write_blocking(SPI_PORT, &spiData, 1);
}


void command(uint8_t c) {

	//digitalWrite(_dc, LOW);
	gpio_put(LCD_DC, 0);

	//digitalWrite(_cs, LOW);
	gpio_put(LCD_CS, 0);
	spiWrite(c);
	gpio_put(LCD_CS, 1);
}

void data(uint8_t c) {
	//digitalWrite(_dc, HIGH);
	gpio_put(LCD_DC, 1);

	//digitalWrite(_cs, LOW);
	gpio_put(LCD_CS, 0);
	spiWrite(c);

	//digitalWrite(_cs, HIGH);
	gpio_put(LCD_CS, 1);
}

void setContrast(uint8_t val) {
	if (val > 0x7f) {
		val = 0x7f;
	}
	command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );
	command( PCD8544_SETVOP | val);
	command(PCD8544_FUNCTIONSET);

}



void display(void) {
	uint8_t col, maxcol, p;

	for(p = 0; p < 6; p++) {
#ifdef enablePartialUpdate
		// check if this page is part of update
		if ( yUpdateMin >= ((p+1)*8) ) {
			continue;   // nope, skip it!
		}
		if (yUpdateMax < p*8) {
			break;
		}
#endif

		command(PCD8544_SETYADDR | p);


#ifdef enablePartialUpdate
		col = xUpdateMin;
		maxcol = xUpdateMax;
#else
		// start at the beginning of the row
		col = 0;
		maxcol = LCDWIDTH-1;
#endif

		command(PCD8544_SETXADDR | col);

		//digitalWrite(_dc, HIGH);
		gpio_put(LCD_DC, 1);

		//digitalWrite(_cs, LOW);
		gpio_put(LCD_CS, 0);
		for(; col <= maxcol; col++) {
			spiWrite(pcd8544_buffer[(LCDWIDTH*p)+col]);
		}

		//digitalWrite(_cs, HIGH);
		gpio_put(LCD_CS, 1);

	}

	command(PCD8544_SETYADDR );  // no idea why this is necessary but it is to finish the last byte?
#ifdef enablePartialUpdate
	xUpdateMin = LCDWIDTH - 1;
	xUpdateMax = 0;
	yUpdateMin = LCDHEIGHT-1;
	yUpdateMax = 0;
#endif

}

// clear everything
void clearDisplay(void) {
	memset(pcd8544_buffer, 0, LCDWIDTH*LCDHEIGHT/8);
	updateBoundingBox(0, 0, LCDWIDTH-1, LCDHEIGHT-1);
	cursor_y = cursor_x = 0;
}

void writePixel(int16_t x, int16_t y, uint16_t color){
	drawPixel(x, y, color);
}

void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color){
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		_swap_int16_t(x0, y0);
		_swap_int16_t(x1, y1);
	}

	if (x0 > x1) {
		_swap_int16_t(x0, x1);
		_swap_int16_t(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0<=x1; x0++) {
		if (steep) {
			writePixel(y0, x0, color);
		} else {
			writePixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void drawFastVLine(int16_t x, int16_t y,int16_t h, uint16_t color){
	writeLine(x, y, x, y+h-1, color);
}

void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color){
	writeLine(x, y, x+w-1, y, color);
}
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color) {
	//startWrite();
	for (int16_t i=x; i<x+w; i++) {
		writeFastVLine(i, y, h, color);
	}
	//endWrite();
}
void fillScreen(uint16_t color) {
	fillRect(0, 0, _width, _height, color);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color) {
	// Update in subclasses if desired!
	if(x0 == x1){
		if(y0 > y1) _swap_int16_t(y0, y1);
		drawFastVLine(x0, y0, y1 - y0 + 1, color);
	} else if(y0 == y1){
		if(x0 > x1) _swap_int16_t(x0, x1);
		drawFastHLine(x0, y0, x1 - x0 + 1, color);
	} else {
		//startWrite();
		writeLine(x0, y0, x1, y1, color);
		//endWrite();
	}
}

void drawCircle(int16_t x0, int16_t y0, int16_t r,uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	//startWrite();
	writePixel(x0  , y0+r, color);
	writePixel(x0  , y0-r, color);
	writePixel(x0+r, y0  , color);
	writePixel(x0-r, y0  , color);

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		writePixel(x0 + x, y0 + y, color);
		writePixel(x0 - x, y0 + y, color);
		writePixel(x0 + x, y0 - y, color);
		writePixel(x0 - x, y0 - y, color);
		writePixel(x0 + y, y0 + x, color);
		writePixel(x0 - y, y0 + x, color);
		writePixel(x0 + y, y0 - x, color);
		writePixel(x0 - y, y0 - x, color);
	}
	//endWrite();
}
void writeFastVLine(int16_t x, int16_t y,int16_t h, uint16_t color) {
    // Overwrite in subclasses if startWrite is defined!
    // Can be just writeLine(x, y, x, y+h-1, color);
    // or writeFillRect(x, y, 1, h, color);
    drawFastVLine(x, y, h, color);
}
void writeFastHLine(int16_t x, int16_t y,int16_t w, uint16_t color) {
    // Overwrite in subclasses if startWrite is defined!
    // Example: writeLine(x, y, x+w-1, y, color);
    // or writeFillRect(x, y, w, 1, color);
    drawFastHLine(x, y, w, color);
}
void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    // Overwrite in subclasses if desired!
    fillRect(x,y,w,h,color);
}


void drawCircleHelper( int16_t x0, int16_t y0,int16_t r, uint8_t cornername, uint16_t color) {
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x4) {
			writePixel(x0 + x, y0 + y, color);
			writePixel(x0 + y, y0 + x, color);
		}
		if (cornername & 0x2) {
			writePixel(x0 + x, y0 - y, color);
			writePixel(x0 + y, y0 - x, color);
		}
		if (cornername & 0x8) {
			writePixel(x0 - y, y0 + x, color);
			writePixel(x0 - x, y0 + y, color);
		}
		if (cornername & 0x1) {
			writePixel(x0 - y, y0 - x, color);
			writePixel(x0 - x, y0 - y, color);
		}
	}
}
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {

	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;

		if (cornername & 0x1) {
			writeFastVLine(x0+x, y0-y, 2*y+1+delta, color);
			writeFastVLine(x0+y, y0-x, 2*x+1+delta, color);
		}
		if (cornername & 0x2) {
			writeFastVLine(x0-x, y0-y, 2*y+1+delta, color);
			writeFastVLine(x0-y, y0-x, 2*x+1+delta, color);
		}
	}
}
void fillCircle(int16_t x0, int16_t y0, int16_t r,uint16_t color) {
	//startWrite();
	writeFastVLine(x0, y0-r, 2*r+1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
	//endWrite();
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color) {
	//startWrite();
	writeFastHLine(x, y, w, color);
	writeFastHLine(x, y+h-1, w, color);
	writeFastVLine(x, y, h, color);
	writeFastVLine(x+w-1, y, h, color);
	//endWrite();
}

void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
	// smarter version
	//startWrite();
	writeFastHLine(x+r  , y    , w-2*r, color); // Top
	writeFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
	writeFastVLine(x    , y+r  , h-2*r, color); // Left
	writeFastVLine(x+w-1, y+r  , h-2*r, color); // Right
	// draw four corners
	drawCircleHelper(x+r    , y+r    , r, 1, color);
	drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
	drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
	drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
	//endWrite();
}

void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
	// smarter version
	//startWrite();
	writeFillRect(x+r, y, w-2*r, h, color);

	// draw four corners
	fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
	fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
	//endWrite();
}

void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
	drawLine(x0, y0, x1, y1, color);
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x0, y0, color);
}

void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

	int16_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1) {
		_swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
	}
	if (y1 > y2) {
		_swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
	}
	if (y0 > y1) {
		_swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
	}

	//startWrite();
	if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if(x1 < a)      a = x1;
		else if(x1 > b) b = x1;
		if(x2 < a)      a = x2;
		else if(x2 > b) b = x2;
		writeFastHLine(a, y0, b-a+1, color);
		//endWrite();
		return;
	}

	int16_t
	dx01 = x1 - x0,
	dy01 = y1 - y0,
	dx02 = x2 - x0,
	dy02 = y2 - y0,
	dx12 = x2 - x1,
	dy12 = y2 - y1;
	int32_t
	sa   = 0,
	sb   = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if(y1 == y2) last = y1;   // Include y1 scanline
	else         last = y1-1; // Skip it

	for(y=y0; y<=last; y++) {
		a   = x0 + sa / dy01;
		b   = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		 */
		if(a > b) _swap_int16_t(a,b);
		writeFastHLine(a, y, b-a+1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for(; y<=y2; y++) {
		a   = x1 + sa / dy12;
		b   = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		 */
		if(a > b) _swap_int16_t(a,b);
		writeFastHLine(a, y, b-a+1, color);
	}
	//endWrite();
}
/*---------------------Text Related Functions--------------------*/
void drawChar(int16_t x, int16_t y, unsigned char c,uint16_t color, uint16_t bg, uint8_t size){

	if(!gfxFont) { // 'Classic' built-in font

		if((x >= _width)            || // Clip right
				(y >= _height)           || // Clip bottom
				((x + 6 * size - 1) < 0) || // Clip left
				((y + 8 * size - 1) < 0))   // Clip top
			return;

		if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

		//startWrite();
		for(int8_t i=0; i<5; i++ ) { // Char bitmap = 5 columns
			uint8_t line = pgm_read_byte(&font[c * 5 + i]);
			for(int8_t j=0; j<8; j++, line >>= 1) {
				if(line & 1) {
					if(size == 1)
						writePixel(x+i, y+j, color);
					else
						writeFillRect(x+i*size, y+j*size, size, size, color);
				} else if(bg != color) {
					if(size == 1)
						writePixel(x+i, y+j, bg);
					else
						writeFillRect(x+i*size, y+j*size, size, size, bg);
				}
			}
		}
		if(bg != color) { // If opaque, draw vertical line for last column
			if(size == 1) writeFastVLine(x+5, y, 8, bg);
			else          writeFillRect(x+5*size, y, size, 8*size, bg);
		}
		//endWrite();

	} else { // Custom font

		// Character is assumed previously filtered by write() to eliminate
		// newlines, returns, non-printable characters, etc.  Calling
		// drawChar() directly with 'bad' characters of font may cause mayhem!

		c -= (uint8_t)pgm_read_byte(&gfxFont->first);
		GFXglyph *glyph  = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
		uint8_t  *bitmap = (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);

		uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
		uint8_t  w  = pgm_read_byte(&glyph->width),
				h  = pgm_read_byte(&glyph->height);
		int8_t   xo = pgm_read_byte(&glyph->xOffset),
				yo = pgm_read_byte(&glyph->yOffset);
		uint8_t  xx, yy, bits = 0, bit = 0;
		int16_t  xo16 = 0, yo16 = 0;

		if(size > 1) {
			xo16 = xo;
			yo16 = yo;
		}

		// Todo: Add character clipping here

		// NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
		// THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
		// has typically been used with the 'classic' font to overwrite old
		// screen contents with new data.  This ONLY works because the
		// characters are a uniform size; it's not a sensible thing to do with
		// proportionally-spaced fonts with glyphs of varying sizes (and that
		// may overlap).  To replace previously-drawn text when using a custom
		// font, use the getTextBounds() function to determine the smallest
		// rectangle encompassing a string, erase the area with fillRect(),
		// then draw new text.  This WILL infortunately 'blink' the text, but
		// is unavoidable.  Drawing 'background' pixels will NOT fix this,
		// only creates a new set of problems.  Have an idea to work around
		// this (a canvas object type for MCUs that can afford the RAM and
		// displays supporting setAddrWindow() and pushColors()), but haven't
		// implemented this yet.

		//startWrite();
		for(yy=0; yy<h; yy++) {
			for(xx=0; xx<w; xx++) {
				if(!(bit++ & 7)) {
					bits = pgm_read_byte(&bitmap[bo++]);
				}
				if(bits & 0x80) {
					if(size == 1) {
						writePixel(x+xo+xx, y+yo+yy, color);
					} else {
						writeFillRect(x+(xo16+xx)*size, y+(yo16+yy)*size,
								size, size, color);
					}
				}
				bits <<= 1;
			}
		}
		//endWrite();

	} // End classic vs custom font
}
/**************************************************************************/
/*!
    @brief  Print one byte/character of data, used to support print()
    @param  c  The 8-bit ascii character to write
 */
/**************************************************************************/
size_t write(uint8_t c) {
	if(!gfxFont) {

		if(c == '\n') {                        // Newline?
			cursor_x  = 0;                     // Reset x to zero,
			cursor_y += textsize * 8;          // advance y one line
		} else if(c != '\r') {                 // Ignore carriage returns
			if(wrap && ((cursor_x + textsize * 6) > _width)) { // Off right?
				cursor_x  = 0;                 // Reset x to zero,
				cursor_y += textsize * 8;      // advance y one line
			}
			drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
			cursor_x += textsize * 6;          // Advance x one char
		}

	} else { // Custom font

		if(c == '\n') {
			cursor_x  = 0;
			cursor_y += (int16_t)textsize *
					(uint8_t)pgm_read_byte(&gfxFont->yAdvance);
		} else if(c != '\r') {
			uint8_t first = pgm_read_byte(&gfxFont->first);
			if((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
				GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(
						&gfxFont->glyph))[c - first]);
				uint8_t   w     = pgm_read_byte(&glyph->width),
						h     = pgm_read_byte(&glyph->height);
				if((w > 0) && (h > 0)) { // Is there an associated bitmap?
						int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
						if(wrap && ((cursor_x + textsize * (xo + w)) > _width)) {
							cursor_x  = 0;
							cursor_y += (int16_t)textsize *
									(uint8_t)pgm_read_byte(&gfxFont->yAdvance);
						}
						drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
				}
				cursor_x += (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize;
			}
		}

	}
	return 1;
}

void printChar(char c)
{
	write(c);
}
void printString(char string[])
{
	for(uint8_t i=0;  i< 16 && string[i]!=NULL; i++)
	{
		write((uint8_t)string[i]);
	}
}
void printInt(int number)
{
	char numStr[16];
	sprintf(numStr,"%d", number);
	printString(numStr);
}

void printFloat(float number, int decimalPoints)
{
	char numStr[16];
	sprintf(numStr,"%.*f",decimalPoints, number);
	printString(numStr);
}
void setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
}

int16_t const getCursorX(void)  {
    return cursor_x;
}
int16_t const getCursorY(void)  {
    return cursor_y;
}
void setTextSize(uint8_t s) {
    textsize = (s > 0) ? s : 1;
}
void setTextColor(uint16_t c, uint16_t b) {
    textcolor   = c;
    textbgcolor = b;
}
void cp437(bool x) {
    _cp437 = x;
}
void setFont(const GFXfont *f) {
    if(f) {            // Font struct pointer passed in?
        if(!gfxFont) { // And no current font struct?
            // Switching from classic to new font behavior.
            // Move cursor pos down 6 pixels so it's on baseline.
            cursor_y += 6;
        }
    } else if(gfxFont) { // NULL passed.  Current font struct defined?
        // Switching from new to classic font behavior.
        // Move cursor pos up 6 pixels so it's at top-left of char.
        cursor_y -= 6;
    }
    gfxFont = (GFXfont *)f;
}

void charBounds(char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy) {

    if(gfxFont) {

        if(c == '\n') { // Newline?
            *x  = 0;    // Reset x to zero, advance y by one line
            *y += textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        } else if(c != '\r') { // Not a carriage return; is normal char
            uint8_t first = pgm_read_byte(&gfxFont->first),
                    last  = pgm_read_byte(&gfxFont->last);
            if((c >= first) && (c <= last)) { // Char present in this font?
                GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(
                  &gfxFont->glyph))[c - first]);
                uint8_t gw = pgm_read_byte(&glyph->width),
                        gh = pgm_read_byte(&glyph->height),
                        xa = pgm_read_byte(&glyph->xAdvance);
                int8_t  xo = pgm_read_byte(&glyph->xOffset),
                        yo = pgm_read_byte(&glyph->yOffset);
                if(wrap && ((*x+(((int16_t)xo+gw)*textsize)) > _width)) {
                    *x  = 0; // Reset x to zero, advance y by one line
                    *y += textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
                }
                int16_t ts = (int16_t)textsize,
                        x1 = *x + xo * ts,
                        y1 = *y + yo * ts,
                        x2 = x1 + gw * ts - 1,
                        y2 = y1 + gh * ts - 1;
                if(x1 < *minx) *minx = x1;
                if(y1 < *miny) *miny = y1;
                if(x2 > *maxx) *maxx = x2;
                if(y2 > *maxy) *maxy = y2;
                *x += xa * ts;
            }
        }

    } else { // Default font

        if(c == '\n') {                     // Newline?
            *x  = 0;                        // Reset x to zero,
            *y += textsize * 8;             // advance y one line
            // min/max x/y unchaged -- that waits for next 'normal' character
        } else if(c != '\r') {  // Normal char; ignore carriage returns
            if(wrap && ((*x + textsize * 6) > _width)) { // Off right?
                *x  = 0;                    // Reset x to zero,
                *y += textsize * 8;         // advance y one line
            }
            int x2 = *x + textsize * 6 - 1, // Lower-right pixel of char
                y2 = *y + textsize * 8 - 1;
            if(x2 > *maxx) *maxx = x2;      // Track max x, y
            if(y2 > *maxy) *maxy = y2;
            if(*x < *minx) *minx = *x;      // Track min x, y
            if(*y < *miny) *miny = *y;
            *x += textsize * 6;             // Advance x one char
        }
    }
}

void getTextBounds(const char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
    uint8_t c; // Current character

    *x1 = x;
    *y1 = y;
    *w  = *h = 0;

    int16_t minx = _width, miny = _height, maxx = -1, maxy = -1;

    while((c = *str++))
        charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);

    if(maxx >= minx) {
        *x1 = minx;
        *w  = maxx - minx + 1;
    }
    if(maxy >= miny) {
        *y1 = miny;
        *h  = maxy - miny + 1;
    }
}

/**************************************************************************/
/*!
    @brief      Set rotation setting for display
    @param  x   0 thru 3 corresponding to 4 cardinal rotations
*/
/**************************************************************************/
void setRotation(uint8_t x) {
  rotation = (x & 3);
  switch (rotation) {
  case 0:
  case 2:
    _width = LCDWIDTH;
    _height = LCDHEIGHT;
    break;
  case 1:
  case 3:
    _width = LCDHEIGHT;
    _height = LCDWIDTH;
    break;
  }
}
void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) {

  int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  //startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
      if (byte & 0x80)
        writePixel(x + i, y, color);
    }
  }
  //endWrite();
}

/************************************************************************/
  /*!
    @brief      Demo Functions for Nokia 5110 LCD
    @returns    
  */
  /************************************************************************/
void testdrawchar(void) {
  setTextSize(1);
  //setTextColor(BLACK);
  setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    write(i);
    //if ((i > 0) && (i % 14 == 0))
      //printString();
  }    
  display();
}

void testdrawcircle(void) {
  for (int16_t i=0; i<LCDHEIGHT; i+=2) {
    drawCircle(LCDWIDTH/2, LCDHEIGHT/2, i, BLACK);
    display();
  }
}

void testfillrect(void) {
  uint8_t color = 1;
  for (int16_t i=0; i<LCDHEIGHT/2; i+=3) {
    // alternate colors
    fillRect(i, i, LCDWIDTH-i*2, LCDHEIGHT-i*2, color%2);
    display();
    color++;
  }
}

void testdrawroundrect(void) {
  for (int16_t i=0; i<LCDHEIGHT/2-2; i+=2) {
    drawRoundRect(i, i, LCDWIDTH-2*i, LCDHEIGHT-2*i, LCDHEIGHT/4, BLACK);
    display();
  }
}

void testfillroundrect(void) {
  uint8_t color = BLACK;
  for (int16_t i=0; i<LCDHEIGHT/2-2; i+=2) {
    fillRoundRect(i, i, LCDWIDTH-2*i, LCDHEIGHT-2*i, LCDHEIGHT/4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display();
  }
}
   
void testdrawrect(void) {
  for (int16_t i=0; i<LCDHEIGHT/2; i+=2) {
    drawRect(i, i, LCDWIDTH-2*i, LCDHEIGHT-2*i, BLACK);
    display();
  }
}

void testdrawline() {  
  for (int16_t i=0; i<LCDWIDTH; i+=4) {
    drawLine(0, 0, i, LCDHEIGHT-1, BLACK);
    display();
  }
  for (int16_t i=0; i<LCDHEIGHT; i+=4) {
    drawLine(0, 0, LCDWIDTH-1, i, BLACK);
    display();
  }
  sleep_ms(250);
  
  clearDisplay();
  for (int16_t i=0; i<LCDWIDTH; i+=4) {
    drawLine(0, LCDHEIGHT-1, i, 0, BLACK);
    display();
  }
  for (int8_t i=LCDHEIGHT-1; i>=0; i-=4) {
    drawLine(0, LCDHEIGHT-1, LCDWIDTH-1, i, BLACK);
    display();
  }
  sleep_ms(250);
  
  clearDisplay();
  for (int16_t i=LCDWIDTH-1; i>=0; i-=4) {
    drawLine(LCDWIDTH-1, LCDHEIGHT-1, i, 0, BLACK);
    display();
  }
  for (int16_t i=LCDHEIGHT-1; i>=0; i-=4) {
    drawLine(LCDWIDTH-1, LCDHEIGHT-1, 0, i, BLACK);
    display();
  }
  sleep_ms(250);

  clearDisplay();
  for (int16_t i=0; i<LCDHEIGHT; i+=4) {
    drawLine(LCDWIDTH-1, 0, 0, i, BLACK);
    display();
  }
  for (int16_t i=0; i<LCDWIDTH; i+=4) {
    drawLine(LCDWIDTH-1, 0, i, LCDHEIGHT-1, BLACK); 
    display();
  }
  sleep_ms(250);
}

void LCD_Demo(void)
{
    setRotation(2);
    // draw a single pixel
    drawPixel(10, 10, BLACK);
    display();
    sleep_ms(2000);
    clearDisplay();

    // draw many lines
    testdrawline();
    display();
    sleep_ms(2000);
    clearDisplay();

    // draw rectangles
    testdrawrect();
    display();
    sleep_ms(2000);
    clearDisplay();

    // draw multiple rectangles
    testfillrect();
    display();
    sleep_ms(2000);
    clearDisplay();

    // draw mulitple circles
    testdrawcircle();
    display();
    sleep_ms(2000);
    clearDisplay();

    // draw a circle, 10 pixel radius
    fillCircle(LCDWIDTH / 2, LCDHEIGHT / 2, 10, BLACK);
    display();
    sleep_ms(2000);
    clearDisplay();

    testdrawroundrect();
    sleep_ms(2000);
    clearDisplay();

    testfillroundrect();
    sleep_ms(2000);
    clearDisplay();

    // draw the first ~12 characters in the font
    testdrawchar();
    display();
    sleep_ms(2000);
    
    clearDisplay();
    setCursor(0, 0);
    setTextSize(1);
    printString("Welcome To");
    setCursor(0, 10);
    setTextSize(1);
    printString("ViduraEmbedded");
    setCursor(0, 20);
    setTextSize(1);
    printString("Subscribe to ");
    setCursor(0, 30);
    setTextSize(1);
    printString("My Channel ");
    display();
}
