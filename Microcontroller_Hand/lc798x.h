#ifndef LC798X_H
#define LC798X_H

#include <stdio.h>
#include <stdlib.h>

#include <lpc17xx.h>

// API definitions 
#define LCD_CURSOR_OFF 				0x00
#define LCD_CURSOR_LINE_STEADY		0x01
#define LCD_CURSOR_CHAR_BLINK		0x02
#define LCD_CURSOR_LINE_BLINK		0x03


#define LCD_GRAPHICS		2
#define LCD_FONT_EXTERNAL	1
#define	LCD_FONT_INTERNAL	0
#define LCD_ON 				4
#define LCD_OFF				0

typedef struct 
{
  uint8_t x;
  uint8_t y;
} xy8_t;

typedef struct 
{
  uint16_t x;
  uint16_t y;
} xy16_t;


// print a single character, interpreting control characters
//int lcd_putchar(char c, FILE *stream);
int lcd_putchar(char c);

// API fucntions, general
void 	lcd_init_interface(void);
void 	lcd_clear_screen(void);
uint8_t lcd_set_cursor_mode(uint8_t mode, uint8_t sy);
void 	lcd_set_cursor(uint8_t x, uint8_t y);
void 	lcd_newline(void);



// API functions for graphics mode
void lcd_write_pixel(uint16_t x, uint16_t y, uint8_t v, uint8_t r);
void lcd_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t v, uint8_t r);
void lcd_ellipse(uint16_t xm, uint16_t ym, uint16_t a, uint16_t b, uint8_t v, uint8_t r);


// API functions, controller specific
uint8_t lcd_set_system(uint8_t fg, uint8_t fx, uint8_t fy);
void 	lcd_control(uint8_t enable);


// stream for LCD
extern FILE lcd_stream;


 
#endif




