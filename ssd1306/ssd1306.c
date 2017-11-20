/*	
 *	SSD1306.h
 *	
 *	I2C based driver library for SSD1306 based 128x64 OLED display module for AVR MCUs
 *
 *	Date: Sept 21, 2017
 *	Author: Visakhan
 */

#include <avr/pgmspace.h>
#include "avr_twi.h"
#include "ssd1306.h"


static const uint8_t * current_font;
static uint8_t 	font_page;
static uint8_t 	font_column;

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

uint8_t oled_init(void)
{
	/* Initialize AVR TWI bus */
	TWI_Init();
	/* Initialize SSD1306 display */
	return oled_send_buf(oled_init_cmds, sizeof(oled_init_cmds));	
}

uint8_t oled_horizontal_mode(uint8_t pages, uint8_t col_start, uint8_t col_end)
{
	uint8_t tx_buf[9] = {
		OLED_CONTROL_BYTE_CMD_STREAM,
		OLED_CMD_SET_ADDR_MODE,	/* Set Horizontal addressing mode */
		0x00,
		OLED_CMD_SET_COL_ADDR,	/* Set column start and end address */
		col_start,
		col_end,
		OLED_CMD_SET_PAGE_ADDR,	/* Set page start and end address */
		pages & 0xF,
		pages >> 4
	};
	
	return oled_send_buf(tx_buf, sizeof(tx_buf));
}


/* Activate Page Addressing mode and set Page & Starting Column */
uint8_t oled_page_mode(uint8_t page, uint8_t col_start)
{
	uint8_t 		tx_data[6];
	
	tx_data[0] = OLED_CONTROL_BYTE_CMD_STREAM;
	tx_data[1] = OLED_CMD_SET_ADDR_MODE;
	tx_data[2] = OLED_PAGE_ADDRESSING;
	tx_data[3] = OLED_CMD_SET_PAGE_START(page);
	tx_data[4] = OLED_CMD_SET_COL_START_LOWER(col_start);
	tx_data[5] = OLED_CMD_SET_COL_START_HIGHER(col_start >> 4);
	
	return oled_send_buf(tx_data, sizeof(tx_data));
}


/********************** UTILITY FUNCTIONS *************************/

/* Send a buffer to display
   NOTE: buf[0] should contain the Control byte: OLED_CONTROL_BYTE_DATA_STREAM 
		 Display data starts at buf[1]
		 size : Size of the whole buffer
 */
uint8_t oled_display_data(uint8_t *buf, uint8_t size)
{
	return oled_send_buf(buf, size);
}



void oled_set_font(const uint8_t *font_data)
{
	current_font = font_data;
}

void oled_set_position(uint8_t page, uint8_t column)
{
	font_page = page;
	font_column = column;
}


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
	//oled_horizontal_mode(OLED_PAGE_RANGE(font_page, font_page+height-1), font_column, font_column+width);
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


uint8_t oled_putstring(char *str)
{
	while(*str) {
		if(oled_putchar(*str++)) {
			return 1;
		}
	}
	return 0;
}


void oled_number(uint16_t num, uint8_t dotPos)
{
	uint8_t i=0;
	char arr[5] = {};
	uint16_t p;
	
	do {	
		p = num/10;
		arr[i++] =  num - p*10;
		num = p;
	} while(p);
	
	while(i--) {
		oled_putchar(arr[i]+48);
		if(dotPos && (dotPos == i) )
			oled_putchar('.');
	}
}


/* 	Display 8-bit Numbers	*/
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


uint8_t oled_get_column(void)
{
	return font_column;
}