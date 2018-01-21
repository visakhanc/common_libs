/*	
 *	SSD1306.h
 *	
 *	I2C based driver library for SSD1306 based 128x64 OLED display module for AVR MCUs
 *	This is a simple library with functions to display text and graphic utilities to draw lines
 *
 *	Date: Sept 21, 2017
 *	Author: Visakhan C
 */

#include <avr/pgmspace.h>
#include "avr_twi.h"
#include "ssd1306.h"


/**************************** VARIABLES ********************************/

static uint8_t _page_buf[129] = {OLED_CONTROL_BYTE_DATA_STREAM};  /* Buffer to be used for stream data communication */
static const uint8_t * current_font;	/* Points to the current font data in flash */
static uint8_t  current_font_height;		/* Font height as number of pages(rows) */
static uint8_t 	font_page;				/* Next page to be written */
static uint8_t 	font_column;			/* Next column to be written */

static twi_params_t		twi_params = 
	{ 	.slave_addr = OLED_I2C_SLA_ADDR };


static uint8_t oled_init_cmds[] = {
	OLED_CONTROL_BYTE_CMD_STREAM,
	OLED_CMD_SET_MUX_RATIO,			/* Set 64 Multiplexer ratio (default) */
	0x3F,
	OLED_CMD_DISPLAY_OFFSET,			/* Set display offset to 0 (default) */
	0x00,
	OLED_CMD_SET_START_LINE(0),		/* Display RAM start line = 0 (default) */
	OLED_CMD_SET_SEG_REMAP(0),		/* No Remap of segments (default) */
	OLED_CMD_COM_SCAN_NORMAL,		/* Normal column scan COM0 to COM63 (default) */
	OLED_CMD_COM_PIN_CONFIG,			/* Set COM pin config: 0x12(default) - Alternate COM pins, no Left/Right remap */
	0x12,
	OLED_CMD_SET_CONTRAST,			/* Set default contrast (0x7F) */
	0x7F,
	OLED_CMD_DISPLAY_NORMAL,			/* Display follows RAM (default) */
	OLED_CMD_INVERSE_OFF,			/* Inverse display OFF (default) */
	OLED_CMD_SET_CLOCK,				/* Set clock divider and oscillator frequency: 0x80 (default) */
	0x80,
	OLED_CMD_SET_CHARGE_PUMP,		/* Enable charge pump */
	0x14,
	OLED_CMD_DISPLAY_ON				/* Turn on display */
};


static uint8_t oled_horizontal_mode_cmd[9] = {
		OLED_CONTROL_BYTE_CMD_STREAM,
		OLED_CMD_SET_ADDR_MODE,	/* Set Horizontal addressing mode */
		0x00,
		OLED_CMD_SET_COL_ADDR,	/* Set column start and end address */
		0, // col_start
		0, // col_end
		OLED_CMD_SET_PAGE_ADDR,	/* Set page start and end address */
		0, // page start
		0  // page end
};


static uint8_t oled_reset_range_cmd[7] = {
		OLED_CONTROL_BYTE_CMD_STREAM,
		OLED_CMD_SET_PAGE_ADDR,	/* To Set Row range to the entire enage (ie, 0 to 7) */
		0,  // page start
		7,  // page end
		OLED_CMD_SET_COL_ADDR,  // To Set Column range to the whole line (ie, 0 to 127)
		0,                      // This is required, if previous 'horizontal' mode used a narrow column range
		0x7F,                   //  which did not include the starting column of 'this' page mode
};


static uint8_t oled_page_mode_cmd[6] = {
		OLED_CONTROL_BYTE_CMD_STREAM,
		OLED_CMD_SET_ADDR_MODE,
		OLED_PAGE_ADDRESSING,
		0, //OLED_CMD_SET_PAGE_START()
		0, //OLED_CMD_SET_COL_START_LOWER()
		0, //OLED_CMD_SET_COL_START_HIGHER()
};




/******************************* PRIVATE FUNCTIONS ***********************************/

static uint8_t oled_send_buf(uint8_t *buf, uint8_t len)
{
	twi_params.tx_buf = buf;
	twi_params.tx_count = len;
	if(TWI_Master_Transfer(&twi_params) != TWI_STATUS_DONE) {
		return 1;
	}
	return 0;
}

uint8_t oled_command(uint8_t cmd)
{
	uint8_t tx_data[2];
	
	tx_data[0] = OLED_CONTROL_BYTE_CMD;
	tx_data[1] = cmd;
	return oled_send_buf(tx_data, sizeof(tx_data));
}

uint8_t oled_data(uint8_t data)
{
	uint8_t tx_data[2];
	
	tx_data[0] = OLED_CONTROL_BYTE_DATA;
	tx_data[1] = data;
	return oled_send_buf(tx_data, sizeof(tx_data));
}

/* Initialize OLED display */
uint8_t oled_init(void)
{
	/* Initialize AVR TWI bus */
	TWI_Init();
	/* Initialize SSD1306 display */
	return oled_send_buf(oled_init_cmds, sizeof(oled_init_cmds));	
}

/* Change to Horizontal addressing mode with specified page and column range */
uint8_t oled_horizontal_mode(uint8_t pages, uint8_t col_start, uint8_t col_end)
{
	oled_horizontal_mode_cmd[4] = col_start;
	oled_horizontal_mode_cmd[5] = col_end;
	oled_horizontal_mode_cmd[7] = pages & 0xF;
	oled_horizontal_mode_cmd[8] = pages >> 4;
	
	return oled_send_buf(oled_horizontal_mode_cmd, sizeof(oled_horizontal_mode_cmd));
}

/* Reset the Page and Column range to the entire display */
static uint8_t oled_reset_range(void)
{
	return oled_send_buf(oled_reset_range_cmd, sizeof(oled_reset_range_cmd));
}


/* Activate Page Addressing mode and set Page & Starting Column */
uint8_t oled_page_mode(uint8_t page, uint8_t col_start)
{
	oled_page_mode_cmd[3] = OLED_CMD_SET_PAGE_START(page);
	oled_page_mode_cmd[4] = OLED_CMD_SET_COL_START_LOWER(col_start);
	oled_page_mode_cmd[5] = OLED_CMD_SET_COL_START_HIGHER(col_start >> 4);
	
	return oled_send_buf(oled_page_mode_cmd, sizeof(oled_page_mode_cmd));
}


/********************** UTILITY FUNCTIONS *************************/



uint8_t oled_display_data(uint8_t *buf, uint8_t size)
{
	return oled_send_buf(buf, size);
}


void oled_clear_area(uint8_t page_range, uint8_t col_start, uint8_t col_end)
{
	uint8_t i;

	if(col_start > col_end) {
		return;
	}
	for( i = 1; i < sizeof(_page_buf); i++) {
		_page_buf[i] = 0;
	}
	for(i = page_range & 0xF; i <= (page_range >> 4); i++) {
		oled_page_mode(i, col_start);
		oled_display_data(_page_buf, (col_end - col_start + 2));
	}

}


void oled_set_font(const uint8_t *font_data)
{
	current_font = font_data;
	current_font_height = pgm_read_byte(current_font) >> 3;
}



void oled_set_position(uint8_t page, uint8_t column)
{
	font_page = page;
	font_column = column;
}



uint8_t oled_get_column(void)
{
	return font_column;
}


#if 0
uint8_t oled_putchar(char c)
{
	const uint8_t *fontPtr = current_font;
	uint8_t height = pgm_read_byte(fontPtr++) >> 3;
	uint8_t width; 
	uint16_t index;
	uint8_t firstChar = pgm_read_byte(fontPtr++);
	uint8_t charCount = pgm_read_byte(fontPtr++);

	if((c < firstChar) || (c >= (firstChar+charCount))) {
		return 1;
	}
	c -= firstChar;
	width = pgm_read_byte(fontPtr+c);
	if(!width) {
		return 0;
	}
	index = 0;
	for(uint8_t i = 0; i < c; i++) {
		index += pgm_read_byte(fontPtr+i) * height;
	}
	fontPtr += (charCount+index);
	index = height;
	while(index--) {
		oled_page_mode(font_page, font_column);
		for(uint8_t i = 0; i < width; i++) {
			oled_data(pgm_read_byte(fontPtr++));
		}
		oled_data(0); // 1 pixel empty space between characters
		font_page++;
	}
	/* update position */
	font_page -= height;
	font_column += width+1;

	return 0;
}
#endif



uint8_t oled_putchar(char c)
{
	const uint8_t *fontPtr = current_font;
	uint8_t height = pgm_read_byte(fontPtr++) >> 3;
	uint8_t width;
	uint16_t index;
	uint8_t firstChar = pgm_read_byte(fontPtr++);
	uint8_t charCount = pgm_read_byte(fontPtr++);
	uint8_t cnt;
	
	if((c < firstChar) || (c >= (firstChar+charCount))) {
		return 1;
	}
	c -= firstChar;
	width = pgm_read_byte(fontPtr+c);
	if(!width) {
		return 0;
	}
	index = 0;
	for(uint8_t i = 0; i < c; i++) {
		index += pgm_read_byte(fontPtr+i) * height;
	}
	fontPtr += (charCount+index);
	oled_horizontal_mode(OLED_PAGE_RANGE(font_page, font_page+height-1), font_column, font_column+width);
	cnt = 1;
	while(height--) {
		for(uint8_t i = 0; i < width; i++) {
			_page_buf[cnt++] = pgm_read_byte(fontPtr++);
		}
		_page_buf[cnt++] = 0;
	}
	oled_display_data(_page_buf, cnt);
	/* Reset Row and Column range to the entire display, after using horizontal mode */
	oled_reset_range();
	/* update position */
	font_column += width+1;
	

	return 0;
}



uint8_t oled_putstring(char *str)
{
	while(*str) {
		if(oled_putchar(*str++)) {
			return 1;
		}
	}
	return 0;
}



void oled_number(int16_t num, uint8_t dotPos)
{
	uint8_t i=0;
	char arr[5] = {};
	uint16_t p;
	
	if(num < 0) {
		num = -num;
		oled_putchar('-');
	}
	do {	
		p = num/10;
		arr[i++] =  num - p*10;
		num = p;
	} while(p);
	/* If digits are less than dot position, show enough leading zeroes after adding Decimal point */
	if(i <= dotPos) {
		oled_putchar('0');
		oled_putchar('.');
		while(dotPos > i) {
			oled_putchar('0');
			dotPos--;
		}
	}
	/* Show digits */
	while(i--) {
		oled_putchar(arr[i]+48);
		if(dotPos && (dotPos == i))
			oled_putchar('.');
	}
}



void oled_number_small(uint8_t num)
{
	uint8_t d=0;
	d = num/100;
	if(d)
		oled_putchar(d+48);
	num -= (d*100);
	d = num/10;
	oled_putchar(d+48);
	oled_putchar( (num%10)+48 );

}



void oled_number_hex(uint16_t num)
{
	uint8_t shift = 16;
	uint8_t c;					
	
	oled_putchar('0');
	oled_putchar('x');
	do {
		shift -= 4;
		c = (uint8_t)((num >> shift) & 0xF);
		if(c > 9) {
			c -= 10;
			oled_putchar('A' + c);
		}
		else {
			oled_putchar(c+48);
		}
	} while(shift);
}



void oled_update_number(struct oled_disp_struct *disp_struct, int16_t num, uint8_t dot_pos)
{
	oled_set_position(disp_struct->row_start, disp_struct->col_start);
	oled_number(num, dot_pos);
	oled_clear_area(OLED_PAGE_RANGE(disp_struct->row_start, disp_struct->row_start+current_font_height-1), oled_get_column(), disp_struct->current_col);
	disp_struct->current_col = oled_get_column();
}



void oled_update_number_hex(struct oled_disp_struct *disp_struct, uint16_t num)
{
	oled_set_position(disp_struct->row_start, disp_struct->col_start);
	oled_number_hex(num);
	oled_clear_area(OLED_PAGE_RANGE(disp_struct->row_start, disp_struct->row_start+current_font_height-1), oled_get_column(), disp_struct->current_col);
	disp_struct->current_col = oled_get_column();
}



void oled_update_text(struct oled_disp_struct *disp_struct, char *text)
{
	oled_set_position(disp_struct->row_start, disp_struct->col_start);
	oled_putstring(text);
	oled_clear_area(OLED_PAGE_RANGE(disp_struct->row_start, disp_struct->row_start+current_font_height-1), oled_get_column(), disp_struct->current_col);
	disp_struct->current_col = oled_get_column();
}




/********** Graphic Utilities *************/


void oled_horizontal_line(uint8_t x1, uint8_t x2, uint8_t y)
{
	uint8_t i;
	uint8_t page = y >> 3;	/* y/8 */

	/* x1 should be less than or equal to x2 */
	if(x1 > x2) {
		i = x1;
		x1 = x2;
		x2 = i;
	}
	oled_page_mode(page, x1);
	y -= (page << 3);	/* page*8 */
	i = 1;
	do {
		_page_buf[i++] = (1 << y);
		x1++;
	} while(x1 <= x2);
	oled_display_data(_page_buf, i);

}



void oled_vertical_line(uint8_t x, uint8_t y1, uint8_t y2)
{
	uint8_t pattern;
	uint8_t page;
	uint8_t pos;

	/* y1 should be less than or equal to y2 */
	if(y1 > y2) {
		pos = y1;
		y1 = y2;
		y2 = pos;
	}
	page = y1 >> 3;
	while(y1 <= y2) {
		pos = y1 - (page<<3);
		pattern = ~((1 << pos) - 1);
		oled_page_mode(page, x);
		if(y2 <= ((page << 3) + 7)) {
			pos = y2 - (page << 3);
			pattern &= ((1 << (pos+1)) - 1);
			oled_data(pattern);
			break;
		}
		oled_data(pattern);
		page++;
		y1 = (page << 3);
	}
}



void oled_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	uint16_t m;
	uint16_t temp;
	uint8_t len;
	uint8_t y;
	uint8_t i;
	uint8_t c;
	uint8_t count;

	if(x1 == x2) {	/* Vertical line */
		oled_vertical_line(x1, y1, y2);
	}
	else if(y1 == y2) {	/* Horizontal line */
		oled_horizontal_line(x1, x2, y1);
	}
	else {	/* Line with a slope */
		/* x1 should be less than or equal to x2 */
		if(x1 > x2) {
			i = x1;
			x1 = x2;
			x2 = i;
			i = y1;
			y1 = y2;
			y2 = i;
		}
		if(y1 > y2) { /* Negative slope */
			m = ((y1 - y2) <<  8)/(x2 - x1);
		}
		else { /* Positive slope */
			m = ((y2 - y1) << 8)/(x2 - x1);
		}
		count = 0;
		c = y1;
		len = x2 - x1;
		for(i = 1; i <= len; i++) {
			if(c > y2) { /* Negative slope */
				temp = (c << 8) - m*i;
			}
			else {  /* Positive slope */
				temp = (c << 8) + m*i;
			}
			y = (uint8_t)(temp >> 8);
			if((temp - (y << 8)) > (0x80)) {
				y++;
			}
			if(y == y1) {
				count++;
				continue;
			}
			if(count > 0) {
				oled_horizontal_line(x1, x1+count, y1);
				x1 += count+1;
			}
			else {
				if(c > y2) { /* Negative slope */
					oled_vertical_line(x1, y1, y+1);
				}
				else { /* Positive slope */
					oled_vertical_line(x1, y1, y-1);
				}
				x1++;
			}
			y1 = y;
			count = 0;
		}
		if(x1 < x2) {
			oled_horizontal_line(x1, x2, y);
		}
	}
}
