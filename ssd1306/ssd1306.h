/*	
 *	SSD1306.h
 *	
 *	I2C based driver library for SSD1306 based 128x64 OLED display module for AVR MCUs
 *
 *	Date: Sept 13, 2017
 *	Author: Visakhan
 */
 
#ifndef SSD1306_H
#define SSD1306_H


/* 7-bit Slave address */
#define SSD_I2C_SLA_ADDR				0x3C	

/* Display dimensions */
#define SSD_DISPLAY_WIDTH				128
#define SSD_DISPLAY_HEIGHT				64

/* Possible Control bytes */
#define SSD_CONTROL_BYTE_DATA_STREAM	0x40
#define SSD_CONTROL_BYTE_DATA			0xC0
#define SSD_CONTROL_BYTE_CMD_STREAM		0x00
#define SSD_CONTROL_BYTE_CMD			0x80

/* Display Addressing modes */
#define SSD_PAGE_ADDRESSING				0x2
#define SSD_VERT_ADDRESSING				0x1
#define SSD_HORIZ_ADDRESSING			0x0

/* Address definitions */
#define SSD_FIRST_PAGE					0
#define SSD_LAST_PAGE					7
#define SSD_PAGE_RANGE_FULL				0x70 	/* Whole(0-7) Page range for Horizontal addressing mode */
#define SSD_FIRST_COLUMN				0
#define SSD_LAST_COLUMN					(SSD_DISPLAY_WIDTH-1)



/*************** COMMANDS ******************/

/* Fundamental Commands */
#define SSD_CMD_SET_CONTRAST			0x81 	/* Set Contrast command: DOUBLE BYTE command - 0x81,A[7:0] (at Reset: 0x7F) */
#define SSD_CMD_DISPLAY_ENTIRE			0xA5	/* Entire display ON (ignore RAM content)*/
#define SSD_CMD_DISPLAY_NORMAL			0xA4	/* Display follows RAM contenet <Reset> */
#define SSD_CMD_INVERSE_ON				0xA7	/* Inverse display: 0:ON, 1:OFF */
#define SSD_CMD_INVERSE_OFF				0xA6	/* Set to normal display from inverse  (0:OFF, 1:ON)  <Reset> */
#define SSD_CMD_DISPLAY_ON				0xAF	/* Display ON */
#define SSD_CMD_DISPLAY_OFF				0xAE	/* Display OFF <Reset> */

/* Addressing setting Commands */
#define SSD_CMD_SET_COL_START_LOWER(x)	((x) & 0xF)				/* Set Lower nibble of Column start address ONLY for Page Addressing */
#define SSD_CMD_SET_COL_START_HIGHER(x)	(((x) & 0xF) | 0x10)	/* Set Higher nibble of Column start address ONLY for Page Addressing */
#define SSD_CMD_SET_ADDR_MODE			0x20					/* Set Addressing mode: DOUBLE BYTE command - 0x20, A[1:0] (Reset: A[1:0] = 10b - Page addressing) */
#define SSD_CMD_SET_COL_ADDR			0x21					/* 3 BYTE COMMAND: Set column start & end address ONLY Horizontal & Vertical addressing mode */
#define SSD_CMD_SET_PAGE_ADDR			0x22 					/* 3 BYTE COMMAND: Set Page start & end address ONLY Horizoontal & Vertical addressing mode */
#define SSD_CMD_SET_PAGE_START(x)		(((x) & 0x7) | 0xB0)	/* Set GDDRAM Page start address (Page0-Page7) ONLY for Page addressing mode */

/* Hardware configuration */
#define SSD_CMD_SET_START_LINE(x)		(((x) & 0x3F) | 0x40)	/* RAM display start line 0 - 63 (default: 0)*/
#define SSD_CMD_SET_SEG_REMAP(x)		(((x) & 0x1) | 0xA0)	/* x = 0 (RESET) COL0 to SEG0; x = 1: COL127 to SEG0 */
#define SSD_CMD_SET_MUX_RATIO			0xA8					/* DOUBLE BYTE command: 0xA8, A[5:0] RESET=0x3F(64MUX) */
#define SSD_CMD_COM_SCAN_NORMAL			0xC0					/* [RESET] Scan from COM0 to COM[N-1] */
#define SSD_CMD_COM_SCAN_REMAPPED		0xC8					/* Scan from COM[N-1] to COM0 */
#define SSD_CMD_DISPLAY_OFFSET			0xD3					/* DOUBLE BYTE command: COM vertical shift (default:0) A[5:0] */
#define SSD_CMD_COM_PIN_CONFIG			0xDA					/* DOUBLE BYTE command: A[5:4]|0x02 A[5]: COM Left/Right remap enable (Default: 0-disabled) A[4]: 1(default) */
#define SSD_CMD_SET_CLOCK				0xD5					/* DOUBLE BYTE command: A[3:0] = clock divider (default:0000b=1), A[7:4]=Osc freq (default:1000b) */
#define SSD_CMD_SET_CHARGE_PUMP			0x8D					/* DOUBLE BYTE command: A[7:4] = 0x14 Enables charge pump */
#define SSD_CMD_SET_CONTRAST			0x81					/* DOUBLE BYTE command: A[7:0] default = 0x7F */
#define SSD_CMD_NOP						0xE3			


/* Function declarations */
uint8_t ssd1306_init(void);
uint8_t ssd1306_data(uint8_t data);
uint8_t ssd1306_command(uint8_t cmd);
uint8_t ssd1306_page_start(uint8_t page, uint8_t column);
uint8_t ssd1306_line(uint8_t length);
uint8_t ssd1306_page_mode(uint8_t page, uint8_t col_start);
uint8_t ssd1306_horizontal_mode(uint8_t pages, uint8_t col_start, uint8_t col_end);
//uint8_t ssd1306_clear_display(void);
uint8_t ssd1306_display_page(uint8_t *page_buf, uint8_t size);

#endif
