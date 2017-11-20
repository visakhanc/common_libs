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
#define OLED_I2C_SLA_ADDR				0x3C	

/* Display dimensions */
#define OLED_DISPLAY_WIDTH				128
#define OLED_DISPLAY_HEIGHT				64

/* Possible Control bytes */
#define OLED_CONTROL_BYTE_DATA_STREAM	0x40
#define OLED_CONTROL_BYTE_DATA			0xC0
#define OLED_CONTROL_BYTE_CMD_STREAM	0x00
#define OLED_CONTROL_BYTE_CMD			0x80

/* Display Addressing modes */
#define OLED_PAGE_ADDRESSING			0x2
#define OLED_VERT_ADDRESSING			0x1
#define OLED_HORIZ_ADDRESSING			0x0

/* Address definitions */
#define OLED_FIRST_PAGE					0
#define OLED_LAST_PAGE					7
#define OLED_PAGE_RANGE_FULL			0x70 	/* Whole(0-7) Page range for Horizontal addressing mode */
#define OLED_FIRST_COLUMN				0
#define OLED_LAST_COLUMN				(OLED_DISPLAY_WIDTH-1)
#define OLED_PAGE_RANGE(from, to)		(((to) << 4)|(from))


/*************** COMMANDS ******************/

/* Fundamental Commands */
#define OLED_CMD_SET_CONTRAST			0x81 	/* Set Contrast command: DOUBLE BYTE command - 0x81,A[7:0] (at Reset: 0x7F) */
#define OLED_CMD_DISPLAY_ENTIRE			0xA5	/* Entire display ON (ignore RAM content)*/
#define OLED_CMD_DISPLAY_NORMAL			0xA4	/* Display follows RAM contenet <Reset> */
#define OLED_CMD_INVERSE_ON				0xA7	/* Inverse display: 0:ON, 1:OFF */
#define OLED_CMD_INVERSE_OFF			0xA6	/* Set to normal display from inverse  (0:OFF, 1:ON)  <Reset> */
#define OLED_CMD_DISPLAY_ON				0xAF	/* Display ON */
#define OLED_CMD_DISPLAY_OFF			0xAE	/* Display OFF <Reset> */

/* Addressing setting Commands */
#define OLED_CMD_SET_COL_START_LOWER(x)	((x) & 0xF)				/* Set Lower nibble of Column start address ONLY for Page Addressing */
#define OLED_CMD_SET_COL_START_HIGHER(x)	(((x) & 0xF) | 0x10)	/* Set Higher nibble of Column start address ONLY for Page Addressing */
#define OLED_CMD_SET_ADDR_MODE			0x20					/* Set Addressing mode: DOUBLE BYTE command - 0x20, A[1:0] (Reset: A[1:0] = 10b - Page addressing) */
#define OLED_CMD_SET_COL_ADDR			0x21					/* 3 BYTE COMMAND: Set column start & end address ONLY Horizontal & Vertical addressing mode */
#define OLED_CMD_SET_PAGE_ADDR			0x22 					/* 3 BYTE COMMAND: Set Page start & end address ONLY Horizoontal & Vertical addressing mode */
#define OLED_CMD_SET_PAGE_START(x)		(((x) & 0x7) | 0xB0)	/* Set GDDRAM Page start address (Page0-Page7) ONLY for Page addressing mode */

/* Hardware configuration */
#define OLED_CMD_SET_START_LINE(x)		(((x) & 0x3F) | 0x40)	/* RAM display start line 0 - 63 (default: 0)*/
#define OLED_CMD_SET_SEG_REMAP(x)		(((x) & 0x1) | 0xA0)	/* x = 0 (RESET) COL0 to SEG0; x = 1: COL127 to SEG0 */
#define OLED_CMD_SET_MUX_RATIO			0xA8					/* DOUBLE BYTE command: 0xA8, A[5:0] RESET=0x3F(64MUX) */
#define OLED_CMD_COM_SCAN_NORMAL		0xC0					/* [RESET] Scan from COM0 to COM[N-1] */
#define OLED_CMD_COM_SCAN_REMAPPED		0xC8					/* Scan from COM[N-1] to COM0 */
#define OLED_CMD_DISPLAY_OFFSET			0xD3					/* DOUBLE BYTE command: COM vertical shift (default:0) A[5:0] */
#define OLED_CMD_COM_PIN_CONFIG			0xDA					/* DOUBLE BYTE command: A[5:4]|0x02 A[5]: COM Left/Right remap enable (Default: 0-disabled) A[4]: 1(default) */
#define OLED_CMD_SET_CLOCK				0xD5					/* DOUBLE BYTE command: A[3:0] = clock divider (default:0000b=1), A[7:4]=Osc freq (default:1000b) */
#define OLED_CMD_SET_CHARGE_PUMP		0x8D					/* DOUBLE BYTE command: A[7:4] = 0x14 Enables charge pump */
#define OLED_CMD_SET_CONTRAST			0x81					/* DOUBLE BYTE command: A[7:0] default = 0x7F */
#define OLED_CMD_NOP					0xE3			


/* Function declarations */
uint8_t oled_init(void);
uint8_t oled_data(uint8_t data);
uint8_t oled_command(uint8_t cmd);
uint8_t oled_page_start(uint8_t page, uint8_t column);
uint8_t oled_line(uint8_t length);
uint8_t oled_page_mode(uint8_t page, uint8_t col_start);
uint8_t oled_horizontal_mode(uint8_t pages, uint8_t col_start, uint8_t col_end);
//uint8_t oled_clear_display(void);

/* Utility functions */
uint8_t oled_display_data(uint8_t *buf, uint8_t size);
void oled_set_font(const uint8_t *font_data);
void oled_set_position(uint8_t page, uint8_t column);
uint8_t oled_putstring(char *str);
uint8_t oled_putchar(char c);
void oled_number(uint16_t num, uint8_t dotPos);
void oled_number_small(uint8_t num);
uint8_t oled_get_column(void);

#endif
