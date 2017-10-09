/*	
 *	SSD1306.h
 *	
 *	I2C based driver library for SSD1306 based 128x64 OLED display module for AVR MCUs
 *
 *	Date: Sept 21, 2017
 *	Author: Visakhan
 */

#include "avr_twi.h"
#include "ssd1306.h"

static twi_params_t		twi_params = 
	{ 	.slave_addr = SSD_I2C_SLA_ADDR };


static uint8_t ssd_init_cmds[] = {
	SSD_CONTROL_BYTE_CMD_STREAM,
	SSD_CMD_SET_MUX_RATIO,			/* Set 64 Multiplexer ratio (default) */
	0x3F,
	SSD_CMD_DISPLAY_OFFSET,			/* Set display offset to 0 (default) */
	0x00,
	SSD_CMD_SET_START_LINE(0),		/* Display RAM start line = 0 (default) */
	SSD_CMD_SET_SEG_REMAP(0),		/* No Remap of segments (default) */
	SSD_CMD_COM_SCAN_NORMAL,		/* Normal column scan COM0 to COM63 (default) */
	SSD_CMD_COM_PIN_CONFIG,			/* Set COM pin config: 0x12(default) - Alternate COM pins, no Left/Right remap */
	0x12,
	SSD_CMD_SET_CONTRAST,			/* Set default contrast (0x7F) */
	0x7F,
	SSD_CMD_DISPLAY_NORMAL,			/* Display follows RAM (default) */
	SSD_CMD_INVERSE_OFF,			/* Inverse display OFF (default) */
	SSD_CMD_SET_CLOCK,				/* Set clock divider and oscillator frequency: 0x80 (default) */
	0x80,
	SSD_CMD_SET_CHARGE_PUMP,		/* Enable charge pump */
	0x14,
	SSD_CMD_DISPLAY_ON				/* Turn on display */
};
	

static uint8_t ssd1306_send_buf(uint8_t *buf, uint8_t len)
{
	twi_params.tx_buf = buf;
	twi_params.tx_count = len;
	if(TWI_Master_Transfer(&twi_params) != TWI_STATUS_DONE) {
		return 1;
	}
	return 0;
}

uint8_t ssd1306_command(uint8_t cmd)
{
	uint8_t tx_data[2];
	
	tx_data[0] = SSD_CONTROL_BYTE_CMD;
	tx_data[1] = cmd;
	return ssd1306_send_buf(tx_data, sizeof(tx_data));
}

uint8_t ssd1306_data(uint8_t data)
{
	uint8_t tx_data[2];
	
	tx_data[0] = SSD_CONTROL_BYTE_DATA;
	tx_data[1] = data;
	return ssd1306_send_buf(tx_data, sizeof(tx_data));
}

uint8_t ssd1306_init(void)
{
	/* Initialize AVR TWI bus */
	TWI_Init();
	/* Initialize SSD1306 display */
	return ssd1306_send_buf(ssd_init_cmds, sizeof(ssd_init_cmds));	
}

uint8_t ssd1306_horizontal_mode(uint8_t pages, uint8_t col_start, uint8_t col_end)
{
	uint8_t tx_buf[9] = {
		SSD_CONTROL_BYTE_CMD_STREAM,
		SSD_CMD_SET_ADDR_MODE,	/* Set Horizontal addressing mode */
		0x00,
		SSD_CMD_SET_COL_ADDR,	/* Set column start and end address */
		col_start,
		col_end,
		SSD_CMD_SET_PAGE_ADDR,	/* Set page start and end address */
		pages & 0xF,
		pages >> 4
	};
	
	return ssd1306_send_buf(tx_buf, sizeof(tx_buf));
}


/* Activate Page Addressing mode and set Page & Starting Column */
uint8_t ssd1306_page_mode(uint8_t page, uint8_t col_start)
{
	uint8_t 		tx_data[6];
	
	tx_data[0] = SSD_CONTROL_BYTE_CMD_STREAM;
	tx_data[1] = SSD_CMD_SET_ADDR_MODE;
	tx_data[2] = SSD_PAGE_ADDRESSING;
	tx_data[3] = SSD_CMD_SET_PAGE_START(page);
	tx_data[4] = SSD_CMD_SET_COL_START_LOWER(col_start);
	tx_data[5] = SSD_CMD_SET_COL_START_HIGHER(col_start >> 4);
	
	return ssd1306_send_buf(tx_data, sizeof(tx_data));
}


/********************** UTILITY FUNCTIONS *************************/

/* Send a page buffer to display
   NOTE: page_buf[0] should contain the Control byte: SSD_CONTROL_BYTE_DATA_STREAM 
		 Display data starts at page_buf[1]
		 size : Size of the whole page_buf
 */
uint8_t ssd1306_display_page(uint8_t *page_buf, uint8_t size)
{
	return ssd1306_send_buf(page_buf, size);
}

