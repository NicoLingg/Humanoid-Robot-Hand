/* 
    Bibliothek für den LC7980/LC7981
	M. Hermann, 2012
*/

#include "lpc17xx.h"
#include "lc798x.h"


// -----------------------------------------------------------------------
// Configuration section, user must supply own data


// Display HW definitions
#define LC978X_CLK 			4000000
#define LCD_DISPLAY_COLS  	192
#define LCD_DISPLAY_LINES	32

// Pins for communication
/*
#define LC798X_WPORT      	PORTA
#define	LC798X_DPORT      	DDRA
#define	LC798X_RPORT      	PINA

#define LC798X_E_0  		(PORTE &= ~(1<<1))
#define LC798X_RS_0 		(PORTE &= ~(1<<3))
#define LC798X_RW_0 		(PORTE &= ~(1<<0))

#define LC798X_E_1  		(PORTE |= (1<<1))
#define LC798X_RS_1 		(PORTE |= (1<<3))
#define LC798X_RW_1 		(PORTE |= (1<<0))
*/

#define PINA7 (LPC_GPIO1->FIOPIN & (1<<15)	<< 7)
#define PINA6 (LPC_GPIO1->FIOPIN & (1<<14)	<< 6)
#define PINA5 (LPC_GPIO1->FIOPIN & (1<<10)	<< 5)
#define PINA4 (LPC_GPIO1->FIOPIN & (1<<9)	<< 4)
#define PINA3 (LPC_GPIO1->FIOPIN & (1<<8)	<< 3)
#define PINA2 (LPC_GPIO1->FIOPIN & (1<<4)	<< 2)
#define PINA1 (LPC_GPIO1->FIOPIN & (1<<1)	<< 1)
#define PINA0 (LPC_GPIO1->FIOPIN & (1<<0)	<< 0)

#define PINA (PINA7 | PINA6 |PINA5 |PINA4 |PINA3 |PINA2 |PINA1 | PINA0)


#define LC798X_WPORT(dat)  	WR_P1_01489_10_14_15(dat)
#define LC798X_DPORT(dat)   DIR_P1_01489_10_14_15(dat)
#define LC798X_RPORT      	PINA  				//RD_P1_01489_10_14_15()
#define LC798X_RS_1 		(LPC_GPIO1->FIOSET |= (1<<16))
#define LC798X_RW_1  		(LPC_GPIO1->FIOSET |= (1<<17))
#define LC798X_E_1  		(LPC_GPIO1->FIOSET |= (1<<18))

#define LC798X_RS_0 		(LPC_GPIO1->FIOCLR |= (1<<16))
#define LC798X_RW_0 		(LPC_GPIO1->FIOCLR |= (1<<17))
#define LC798X_E_0 			(LPC_GPIO1->FIOCLR |= (1<<18))


void WR_P1_01489_10_14_15(uint8_t dat)
{
	if (dat & 0x01)
		LPC_GPIO1->FIOSET = (1<<0);
	else
		LPC_GPIO1->FIOCLR = (1<<0);

	if (dat & 0x02)
		LPC_GPIO1->FIOSET = (1<<1);
	else
		LPC_GPIO1->FIOCLR = (1<<1);

	if (dat & 0x04)
		LPC_GPIO1->FIOSET = (1<<4);
	else
		LPC_GPIO1->FIOCLR = (1<<4);

	if (dat & 0x08)
		LPC_GPIO1->FIOSET = (uint32_t)(1<<8);
	else
		LPC_GPIO1->FIOCLR = (uint32_t)(1<<8);

	if (dat & 0x10)
		LPC_GPIO1->FIOSET = (uint32_t)(1<<9);
	else
		LPC_GPIO1->FIOCLR = (uint32_t)(1<<9);

	if (dat & 0x20)
		LPC_GPIO1->FIOSET = (uint32_t)(1<<10);
	else
		LPC_GPIO1->FIOCLR = (uint32_t)(1<<10);

	if (dat & 0x40)
		LPC_GPIO1->FIOSET = (uint32_t)(1<<14);
	else
		LPC_GPIO1->FIOCLR = (uint32_t)(1<<14);

	if (dat & 0x80)
		LPC_GPIO1->FIOSET = (uint32_t)(1<<15);
	else
		LPC_GPIO1->FIOCLR = (uint32_t)(1<<15);
}

void DIR_P1_01489_10_14_15(uint8_t dat)
{
	if (dat & 0x01)
		LPC_GPIO1->FIODIR |= (1<<0);
	else
		LPC_GPIO1->FIODIR &= ~(1<<0);

	if (dat & 0x02)
		LPC_GPIO1->FIODIR |= (1<<1);
	else
		LPC_GPIO1->FIODIR &= ~(1<<1);
	if (dat & 0x04)
		LPC_GPIO1->FIODIR |= (1<<4);
	else
		LPC_GPIO1->FIODIR &= ~(1<<4);
	if (dat & 0x08)
		LPC_GPIO1->FIODIR |= (1<<8);
	else
		LPC_GPIO1->FIODIR &= ~(1<<8);
	if (dat & 0x10)
		LPC_GPIO1->FIODIR |= (1<<9);
	else
		LPC_GPIO1->FIODIR &= ~(1<<9);
	if (dat & 0x20)
		LPC_GPIO1->FIODIR |= (1<<10);
	else
		LPC_GPIO1->FIODIR &= ~(1<<10);
	if (dat & 0x40)
		LPC_GPIO1->FIODIR |= (1<<14);
	else
		LPC_GPIO1->FIODIR &= ~(1<<14);
	if (dat & 0x80)
		LPC_GPIO1->FIODIR |= (1<<15);
	else
		LPC_GPIO1->FIODIR &= ~(1<<15);

	/*
	LPC_GPIO1->FIODIR0 = (dat & 0x01) ? (1<<0) : ~(1<<0);
	LPC_GPIO1->FIODIR0 = (dat & 0x02) ? (1<<1) : ~(1<<1);
	LPC_GPIO1->FIODIR0 = (dat & 0x04) ? (1<<4) : ~(1<<4);
	LPC_GPIO1->FIODIR  = (dat & 0x08) ? (1<<8) : ~(1<<8);
	LPC_GPIO1->FIODIR  = (dat & 0x10) ? (1<<9) : ~(1<<9);
	LPC_GPIO1->FIODIR  = (dat & 0x20) ? (1<<10) : ~(1<<10);
	LPC_GPIO1->FIODIR  = (dat & 0x40) ? (1<<14) : ~(1<<14);
	LPC_GPIO1->FIODIR  = (dat & 0x80) ? (1<<15) : ~(1<<15);
*/
}
uint8_t  RD_P1_01489_10_14_15(void)
{
	uint8_t dat;

	if (LPC_GPIO1->FIOPIN & (1<<0))
		dat = 0x01;
	else
		dat = 0x00;
	if (LPC_GPIO1->FIOPIN & (1<<1))
		dat |= 0x02;
	else
		dat &= ~0x02;
	if (LPC_GPIO1->FIOPIN & (1<<4))
		dat |= 0x04;
	else
		dat &= ~0x04;
	if (LPC_GPIO1->FIOPIN & (1<<8))
		dat |= 0x08;
	else
		dat &= ~0x08;
	if (LPC_GPIO1->FIOPIN & (1<<9))
		dat |= 0x10;
	else
		dat &= ~0x10;
	if (LPC_GPIO1->FIOPIN & (1<<10))
		dat |= 0x20;
	else
		dat &= ~0x20;
	if (LPC_GPIO1->FIOPIN & (1<<14))
		dat |= 0x40;
	else
		dat &= ~0x40;
	if (LPC_GPIO1->FIOPIN & (1<<15))
		dat |= 0x80;
	else
		dat &= ~0x80;

	/*
	uint8_t dat = (LPC_GPIO1->FIOPIN0 & 0x01) ? 1 : 0;

	LPC_GPIO1->FIOPIN0 = (dat & 0x01) ? (1<<0) : ~(1<<0);
	LPC_GPIO1->FIOPIN0 = (dat & 0x02) ? (1<<1) : ~(1<<1);
	LPC_GPIO1->FIOPIN0 = (dat & 0x04) ? (1<<4) : ~(1<<4);
	LPC_GPIO1->FIOPIN  = (dat & 0x08) ? (1<<8) : ~(1<<8);
	LPC_GPIO1->FIOPIN  = (dat & 0x10) ? (1<<9) : ~(1<<9);
	LPC_GPIO1->FIOPIN  = (dat & 0x20) ? (1<<10) : ~(1<<10);
	LPC_GPIO1->FIOPIN  = (dat & 0x40) ? (1<<14) : ~(1<<14);
	LPC_GPIO1->FIOPIN  = (dat & 0x80) ? (1<<15) : ~(1<<15);
	*/
	return dat;
}




// ----------------------------------------------------------------------
// end of configuration section



// ----------------------------------------------------------------------
// declarations and definitions which are private to this file

//FILE lcd_stream  = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE );		//#todo



typedef struct
{
  uint8_t	options;	// housekeeping and options
  uint8_t 	modereg;	// contents of LC798x mode register (shadow register)
  xy8_t		display;	// physical size of display, e.g. 192*32
  xy8_t		fontsize;	// box size of the font itself
  xy8_t		fontbox;	// box size for a character, including blank padding 
  xy8_t		textbox;	// box size of text screen (full characters)
  xy8_t		rambox;		// box size of text or graphics data in ram (x and/or y may exceed textbox size in case of partial characters)
  uint16_t 	wbase;		// base address of visible window
  xy8_t		cursor;		// cursor position
} _lcd_data;

// Interface definitions for LC798x
#define LC798X_CMD	1
#define LC798X_DATA	0

// Command set of  LC7981
#define LC798X_MODE 		0x00 // main mode selection
#define LC798X_CHARPITCH 	0x01 // character pitch
#define LC798X_NUMCHARS	 	0x02 // number of chars per line
#define LC798X_NUMLINES 	0x03 // number of lines (for single panel display)
#define LC798X_CURLINE 		0x04 // cursor line position (visible cursor)
#define LC798X_DSPSTART_LO	0x08 // display start address (low byte)
#define LC798X_DSPSTART_HI 	0x09 // display start address (high byte)
#define LC798X_MEMRW_LO		0x0a // memory r/w address, low byte
#define LC798X_MEMRW_HI 	0x0b // memory r/w address, high byte
#define LC798X_MEMWRITE		0x0c // write to memory
#define LC798X_MEMREAD		0x0d // read from memory
#define LC798X_BITCLEAR		0x0e // reset bit in memory
#define LC798X_BITSET		0x0f // set bit in memory



// The structure lcd_data holds all information about the current state of the display
_lcd_data lcd_data;

// private declarations

void 	lc798x_wait_for_ready(void);
void 	lcd_write(uint8_t a0, uint8_t data);
uint8_t lcd_read(void);
void 	lc798x_register(uint8_t reg, uint8_t data);




/*
    Prepare interface for the LC798x
	For this chip nothing must be done.
*/

void lcd_init_interface(void)
{

	LPC_GPIO1->FIODIR |= (1<<16);		//RS
	LPC_GPIO1->FIODIR |= (1<<17);		//RW
	LPC_GPIO1->FIODIR |= (1<<18);		//E
	LC798X_E_0;
	LC798X_DPORT(0x00);
}

/*
   This function writes a command or data into the LC798x.
   The routine works for uc clock frequencies up to 16 MHz.
*/

void lcd_write(uint8_t a0, uint8_t data)
{

  lc798x_wait_for_ready();

  // select CMD or DATA
  if (a0==LC798X_CMD)	LC798X_RS_1;
  else					LC798X_RS_0;
  
  LC798X_RW_0;
  LC798X_DPORT(0x0ff);
  LC798X_WPORT(data);			//#opo
 
  LC798X_E_1;
  LC798X_E_1;
  LC798X_E_1;
  LC798X_E_1;  LC798X_E_1;  LC798X_E_1;  LC798X_E_1;  LC798X_E_1;  LC798X_E_1;

  LC798X_E_0;
}



/*
    This function write a control register of the LC798x
*/
void lc798x_register(uint8_t reg, uint8_t data)
{
  lcd_write(LC798X_CMD, reg);
  lcd_write(LC798X_DATA, data);  
}



/*
   This function reads from the LC798x.
   The routine works for uc clock frequencies up to 16 MHz.

*/
uint8_t lcd_read(void)
{
  uint8_t x;

  lc798x_wait_for_ready();

  // reads are done with RS=0
  LC798X_RS_0;
  LC798X_RW_1;
  
  // not really necessary, but we need 90ns setup time anyway ...
  LC798X_DPORT(0x00);
  LC798X_DPORT(0x00);
 
  LC798X_E_1;
  LC798X_E_1;
  LC798X_E_1;

  x=LC798X_RPORT;			//#todo

  LC798X_E_0;

  return x;
}



/*
   Wait until the LC798x is ready to accept new commands or data
   The routine works for uc clock frequencies up to 16 MHz
*/

void lc798x_wait_for_ready(void)
{
  LC798X_E_0;
  LC798X_RW_1;
  LC798X_RS_1;
  LC798X_DPORT(0x00);	// 90ns min address setup time
  LC798X_DPORT(0x00);

  LC798X_DPORT(0x00);	//#op
  LC798X_DPORT(0x00);	//#op


  LC798X_E_1;			// 140ns min data read time
  LC798X_E_1;
  LC798X_E_1;
  LC798X_E_1;
  LC798X_E_1;
  LC798X_E_1;
  LC798X_E_1;


  /*  original
  do  {
  } while (LC798X_RPORT & (1<<7));
*/

  //##op
  /*
   *   ohne das makro
   */
//	do  {
	//} while (LPC_GPIO1->FIOPIN & (1<<15));			//A7
//
	//##op ohne das lesen...macht evtl schwierigkeiten mit 3,3v
	int i,j;
	for(i=0;i<100;i++){
		for (j=0;j<10;j++){
		}
	}

  LC798X_E_0;
}





/*  Set basic font and display parameters 
    This function calculates the required parameters for the LC798x
	according to the physical display dimensions, and the font size used
	There are some constants that must be defined:

    LCD_DISPLAY_COLS:  number of columns, e.g. 192
	LCD_DISPLAY_LINS:  number of lines, e.g. 32

    Parameters:
	fg: LCD_FONT_INTERNAL: internal font (6x8)
	    LCD_FONT_EXTERNAL: external font, fixed in attached ROM, max 8x16

	fx: font size, x, 1-8 for text modes
	fy: font size, y, 1-16 for text modes

	Return value:
	0: error
	1: OK
*/

uint8_t lcd_set_system(uint8_t fg, uint8_t fx, uint8_t fy)
{
  // inital setting of mode that works for both LC7980 and LC7981
  lcd_data.modereg=0x10;

  // some checks
  if ((fg != LCD_FONT_INTERNAL) && (fg != LCD_FONT_EXTERNAL)) 
  {
    // graphics mode
    fx=8;
	fy=1;
	lcd_data.modereg|=(1<<1);
  }
  else
  {
    // check font limitations in text modes
    if ((fx>8) || (fy>16)) return 0;
  }

  // set correct font size for internal font and also set box size
  if (fg == LCD_FONT_INTERNAL)
  {
    fx=6;
	fy=8;
  }
  else
  {
    lcd_data.modereg|=(1<<0);
  }

  // save font information
  lcd_data.options=fg;
  lcd_data.fontsize.x=fx;
  lcd_data.fontsize.y=fy;
  lcd_data.fontbox.x=fx;
  lcd_data.fontbox.y=fy; 


  // initialize display size
  lcd_data.display.x=LCD_DISPLAY_COLS;
  lcd_data.display.y=LCD_DISPLAY_LINES;

  // window base in all modes 0
  lcd_data.wbase=0;

  // calculate text box and textram box sizes
  lcd_data.textbox.x=LCD_DISPLAY_COLS/fx;
  lcd_data.textbox.y=LCD_DISPLAY_LINES/fy;
  lcd_data.rambox.x=lcd_data.textbox.x;
  lcd_data.rambox.y=lcd_data.textbox.y;
  // if there is a partial character left, provide for an extra blank char
  if ((LCD_DISPLAY_COLS%fx) != 0)
  {
    lcd_data.rambox.x++;
  }
  // since the LC798x only allows even numbers (according to the data sheet),
  // provide (another) blank char if the number right now is odd
  if( lcd_data.rambox.x&1) lcd_data.rambox.x++;
 

  // now program into LC798x
  lc798x_register(LC798X_MODE,lcd_data.modereg);
  lc798x_register(LC798X_CHARPITCH, ((fy-1)<<4) + (fx-1));
  lc798x_register(LC798X_NUMCHARS,lcd_data.rambox.x-1);
  lc798x_register(LC798X_NUMLINES,LCD_DISPLAY_LINES-1);
    
  return 1;
} 



/*
	Set the screen layout in memory and on the display
	The parameters are all taken fom the lcd_data structure
*/

void lcd_set_layout(void)
{

  lc798x_register(LC798X_DSPSTART_LO,(uint8_t)lcd_data.wbase);
  lc798x_register(LC798X_DSPSTART_HI,(uint8_t)(lcd_data.wbase>>8));

}


/*
    Set cursor to corresponding memory address in window.
	The cursor position in lcd_data is updated.
	x and y are given in text/graphics coordinates 
*/

void lcd_set_cursor(uint8_t x, uint8_t y)
{
  uint16_t m;
  
  lcd_data.cursor.x=x;
  lcd_data.cursor.y=y;

  m=lcd_data.wbase+y*lcd_data.rambox.x+x;
  
  lc798x_register(LC798X_MEMRW_LO,(uint8_t)m);
  lc798x_register(LC798X_MEMRW_HI,(uint8_t)(m>>8));
}



/*
    Process a newline request
	If the cursor is in the last line a scroll is executed
	Here the new line is cleared offscreen first, if a scroll is to be performed
*/
	
void lcd_newline(void)
{
  uint8_t sx,n,i;

  // save current x-position of cursor
  sx=lcd_data.cursor.x;

  // increment cursor even if in last line
  lcd_data.cursor.y++;

  // set memory cursor to start of new line
  lcd_set_cursor(0,lcd_data.cursor.y);

  // clear new line
  n=lcd_data.rambox.x;
  lcd_write(LC798X_CMD,LC798X_MEMWRITE);
  for (i=0; i<n; i++) lcd_write(LC798X_DATA,0x20);

  // set cursor to previous x position
  lcd_set_cursor(sx,lcd_data.cursor.y);

  // check for scroll
  if (lcd_data.cursor.y==lcd_data.textbox.y)
  {
     lcd_data.cursor.y--;
	 lcd_data.wbase+=lcd_data.rambox.x;
	 lcd_set_layout();
  }
}


/*
    process a single character to be printed on the display
	(including control characters like newline and return)
*/

//int lcd_putchar(char c, FILE *stream)
int lcd_putchar(char c)
{
  if (c=='\n')
  {
     lcd_newline();
  }
  else
  {
    if (c=='\r')
	{
	  lcd_set_cursor(0,lcd_data.cursor.y);
	}
	else
	{
      if (lcd_data.textbox.x==lcd_data.cursor.x)
	  {
	    lcd_data.cursor.x=0;
	    lcd_newline();
	  }
	  // set memory cursor to new location
	  lcd_set_cursor(lcd_data.cursor.x,lcd_data.cursor.y);
	  // and write character to memory
	  lcd_write(LC798X_CMD, LC798X_MEMWRITE);
      lcd_write(LC798X_DATA, c);
	  lcd_data.cursor.x++;
	
    }  
  }
  return 0;
}


/* 
   Reset screen origin to 0, then clear the text or graphics window and 
*/

void lcd_clear_screen(void)
{
  uint16_t i,n;
  uint8_t v;

  lcd_data.wbase=0;
  lcd_set_layout();

  lcd_set_cursor(0,0);
  if (lcd_data.modereg&2) v=0x00;
  else                    v=0x20;

  n=(uint16_t)lcd_data.rambox.x*(uint16_t)lcd_data.rambox.y;
  lcd_write(LC798X_CMD,LC798X_MEMWRITE);

  for (i=0; i<n; i++) lcd_write(LC798X_DATA,v);
  
  lcd_set_cursor(0,0);
}



/*
   Set the properties of the cursor (in text mode only)
   mode: 
    	LCD_CURSOR_CHAR_BLINK:	blinking char
		LCD_CURSOR_LINE_BLINK:  blinking line at pos sy
		LCD_CURSOR_LINE_STEADY: steady line at pos sy
        else                    off

   sy: if 0, bottom line of font box
       else as defined

   return value:
   0: error 
   1: OK
*/

uint8_t lcd_set_cursor_mode(uint8_t mode, uint8_t sy)
{
  // check for gfx mode
  if (lcd_data.modereg&2) return 0;

  // check bounds
  if (sy > lcd_data.fontbox.y) return 0;

  // set default value if requested
  if (sy==0) sy=lcd_data.fontbox.y;

  // set cursor mode
  lcd_data.modereg&=0xf3;
  lcd_data.modereg|=((mode<<2)&0x0c);
  lc798x_register(LC798X_CURLINE,sy-1);
  lc798x_register(LC798X_MODE,lcd_data.modereg);

  return 1;
}



/*
    enable or disable display an set mode
	
	enable: LCD_ON -> on
	        LCD_OFF -> off
*/

void lcd_control(uint8_t enable)
{

  lcd_data.modereg&=0xd3;
  if (enable==LCD_ON) lcd_data.modereg|=0x20;
  lc798x_register(LC798X_MODE, lcd_data.modereg);
}



/* 
    set/clear a single pixel
	This function is very time slow, since it has to send three commands and three data items
	to the LC798x. For horizontal lines, font panning (bitmap copies) and area fills
	one should not use this function.

	x, y: pixel coordinates 

	v:  0: clear, 
	   !0: set

	r:  0: (x,y) relative to absolute adress 0
	    1: (x,y) relative to current window origin
*/

void lcd_write_pixel(uint16_t x, uint16_t y, uint8_t v, uint8_t r)
{
   uint16_t a;
   uint8_t c;

   // calculate byte address of pixel
   a=(x>>3)+y*(LCD_DISPLAY_COLS>>3);
   if (r==1) a+=lcd_data.wbase;

   // set address 
   lc798x_register(LC798X_MEMRW_LO,(uint8_t)a);
   lc798x_register(LC798X_MEMRW_HI,(uint8_t)(a>>8));

   // set/clear pixel

   if (v==0) c=LC798X_BITCLEAR;
   else      c=LC798X_BITSET;
   
   lc798x_register(c, x&7);
}



/*
    Draw a line (one pixel width)
	The code has been taken from wikipedia, Bresenham.
	No clipping is done.

	x0, y0: start of line
	x1, y1: end of line

	v: 0: clear pixel
	   1: set pixel

   	r: 0: (x,y) relative to absolute adress 0
       1: (x,y) relative to current window origin
  	
*/


void lcd_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t v, uint8_t r)
{
  int16_t dx =  abs(x1-x0);
  int16_t sx =  x0<x1 ? 1 : -1;
  int16_t dy = -abs(y1-y0);
  int16_t sy = y0<y1 ? 1 : -1; 
  int16_t err = dx+dy, e2; 
 
  
  do
  { 
    lcd_write_pixel(x0,y0,v,r);
    if (x0==x1 && y0==y1) break;
    e2 = 2*err;
    if (e2 > dy) { err += dy; x0 += sx; } 
    if (e2 < dx) { err += dx; y0 += sy; } 
  } while (1);
}


/*
    Draw an ellipse (one pixel width)
	The code has been taken from wikipedia, Bresenham.
	No clipping is done.

	x, y: center
	a: x-axis "radius"
	b: y-axis radius"

	v: 0: clear pixel
	   1: set pixel

   	r: 0: (x,y) relative to absolute adress 0
       1: (x,y) relative to current window origin
  	
*/

void lcd_ellipse(uint16_t xm, uint16_t ym, uint16_t a, uint16_t b, uint8_t v, uint8_t r)
{
   int16_t dx = 0, dy = b;
   int16_t a2 = a*a, b2 = b*b;
   int16_t err = b2-(2*b-1)*a2, e2; 
 
   do {
       lcd_write_pixel(xm+dx, ym+dy, r, v); /* I. Quadrant */
       lcd_write_pixel(xm-dx, ym+dy, r, v); /* II. Quadrant */
       lcd_write_pixel(xm-dx, ym-dy, r, v); /* III. Quadrant */
       lcd_write_pixel(xm+dx, ym-dy, r, v); /* IV. Quadrant */
 
       e2 = 2*err;
       if (e2 <  (2*dx+1)*b2) { dx++; err += (2*dx+1)*b2; }
       if (e2 > -(2*dy-1)*a2) { dy--; err -= (2*dy-1)*a2; }
   } while (dy >= 0);
 
   while (dx++ < a) 
   {   /* fehlerhafter Abbruch bei flachen Ellipsen (b=1) korrigieren */
       lcd_write_pixel(xm+dx, ym, r, v);
       lcd_write_pixel(xm-dx, ym, r, v); 
   }
}


