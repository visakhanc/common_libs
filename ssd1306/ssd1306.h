/*	
 *	SSD1306.h
 *	
 *	I2C based driver library for SSD1306 based 128x64 OLED display module for AVR MCUs
 *  This is a simple library with functions to display text and graphic utilities to draw lines
 *
 *	Date: Sept 13, 2017
 *	Author: Visakhan C
 */
 
#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

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


/* Structure to be passed into oled_update_xxx functions */
struct oled_disp_struct {
	uint8_t row_start;
	uint8_t col_start;
	uint8_t current_col;
};


/********************* Function declarations ***********************/

uint8_t oled_init(void);
uint8_t oled_data(uint8_t data);
uint8_t oled_command(uint8_t cmd);
uint8_t oled_page_start(uint8_t page, uint8_t column);
//uint8_t oled_line(uint8_t length);
uint8_t oled_page_mode(uint8_t page, uint8_t col_start);
uint8_t oled_horizontal_mode(uint8_t page_range, uint8_t col_start, uint8_t col_end);




/********************* Utility functions *************************/


/**
 * @brief Send a data buffer to display as a data stream
 * @param buf - Display data starting at buf[1]
 *              buf[0] should contain the Control byte: OLED_CONTROL_BYTE_DATA_STREAM
 * @param size - Size of the whole buffer
 * @return 0 - Success
 *         1 - Error
 */
uint8_t oled_display_data(uint8_t *buf, uint8_t size);


/**
 * @brief Clear an area bounded by a page and column range
 * @param page_range - Start and end page defined using OLED_PAGE_RANGE() macro
 * @param col_start - Starting column (0 - 127)
 * @param col_end - Ending column (0 - 127)
 */
void oled_clear_area(uint8_t page_range, uint8_t col_start, uint8_t col_end);


/**
 * @brief Set the font to be used for text
 * @param font_data - Font data array (font_8pt, font16_pt, font_24pt etc.)
 */
void oled_set_font(const uint8_t *font_data);



/**
 * @brief Set the next text location
 * @param page - Page number (0 - 7)
 * @param column - Column number (0 - 127)
 */
void oled_set_position(uint8_t page, uint8_t column);


/**
 * @brief Returns the current text location column
 * @return Column number (0 - 127)
 */
uint8_t oled_get_column(void);


/**
 * Clear the entire display
 */
#define oled_clear_display()	oled_clear_area(OLED_PAGE_RANGE_FULL, OLED_FIRST_COLUMN, OLED_LAST_COLUMN)



/**
 * @brief Writes a character to the current text location
 * @param c - Character to write
 * @return 0 - Success
 *         1 - Error
 */
uint8_t oled_putchar(char c);


/**
 * @brief Writes a string at the current text location
 * @param str - String to write
 * @return 0 - Success
 *         1 - Error
 */
uint8_t oled_putstring(char *str);



/**
 * @brief Writes a 16-bit signed number with optional decimal point
 * @param num - 16-bit signed number
 * @param dotPos - Decimal point position from left (0 = No decimal point)
 */
void oled_number(int16_t num, uint8_t dotPos);


/**
 * @brief Display positive 8-bit Numbers
 * @param num - 8-bit number
 */
void oled_number_small(uint8_t num);



/**
 * @brief Display 16-bit numbers in hexadecimal format
 * @param num - 16-bit number
 */
void oled_number_hex(uint16_t num);



/**
 * @brief Write a number in the display area specified by the 'disp_struct' structure
 *        This function is used to write a number at a predefined location without having to move
 *        the text location using oled_set_position()
 *        To display a number using this function, initialize a oled_disp_struct with desired location
 *                oled_disp_struct oled_num = { .row_start = 6, .col_start = 10 };
 *        Then, for updating the number every time,
 *                oled_update_number(&oled_num, 12345, 0);
 *
 * @param disp_struct - Structure initialized with starting row(0-7) and column(0-127)
 * @param num - 16-bit signed number to display
 * @param dot_pos - Decimal point position (0 - No DP)
 */
void oled_update_number(struct oled_disp_struct *num_struct, int16_t num, uint8_t dot_pos);


/**
 * @brief Write a number in Hex format in the display area specified by the 'disp_struct' structure
 *        This function is used to write a Hex number at a predefined location without having to move
 *        the text location using oled_set_position()
 *        To display a number using this function, initialize a oled_disp_struct with desired location
 *                oled_disp_struct oled_num = { .row_start = 6, .col_start = 10 };
 *        Then, for updating the number every time,
 *                oled_update_number_hex(&oled_num, 12345);
 *
 * @param disp_struct - Structure initialized with starting row(0-7) and column(0-127)
 * @param num - 16-bit number to display
 */
void oled_update_number_hex(struct oled_disp_struct *num_struct, uint16_t num);


/**
 * @brief Write string in the display area specified by the 'disp_struct' structure
 *        This function is used to write a string at a predefined location without having to move
 *        the text location using oled_set_position()
 *        To display a number using this function, initialize a oled_disp_struct with desired location
 *                oled_disp_struct oled_text = { .row_start = 6, .col_start = 10 };
 *        Then, for updating the text every time,
 *                oled_update_text(&oled_num, "Hello");
 *
 * @param disp_struct - Structure initialized with starting row(0-7) and column(0-127)
 * @param text - String to display (Should be NULL terminated)
 */
void oled_update_text(struct oled_disp_struct *disp_struct, char *text);




/********************** Graphic utilities *************************/


/**
 * @brief Draw a horizontal line between two points (x1,y) and (x2,y)
 * @param x1 - X-coordinate of one point (0 - 127)
 * @param x2 - X-coordinate of the other point (0 - 127)
 * @param y - Common y-coordinate (0 - 63)
 */
void oled_horizontal_line(uint8_t x1, uint8_t x2, uint8_t y);


/**
 * @brief Draw a vertical line between two points (x,y1) and (x,y2)
 * @param x - Common X-coordinate (0 - 127)
 * @param y1 - Y-coordinate of one point (0 - 63)
 * @param y2 - Y-coordinate of the other point (0 - 63)
 */
void oled_vertical_line(uint8_t x, uint8_t y1, uint8_t y2);


/**
 * @brief Draw a line between two points (x1,y1) and (x2,y2)
 * @param x1 - X-coordinate of first point (0 - 127)
 * @param y1 - Y-coordinate of first point (0 - 63)
 * @param x2 - X-coordinate of second point (0 - 127)
 * @param y2 - Y-coordinate of second point (0 - 63)
 */
void oled_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);


#endif
