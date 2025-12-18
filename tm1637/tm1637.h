/*
 * tm1637.h
 *
 *	Definition for TM1637 Segment LED driver
 *
 *  Created on: May 19, 2022
 *      Author: Visakhan
 */

#ifndef TM1637_H_
#define TM1637_H_

#include <stdint.h>
#include <stdbool.h>


/* TM1637 Data Commands */

#define TM1637_WRITE_AUTOINC_CMD	0x40
#define TM1637_WRITE_FIXED_CMD		0x44
#define TM1637_READ_AUTOINC_CMD		0x42
#define TM1637_READ_FIXED_CMD		0x46


/* TM1637 Address commands */
#define TM1637_ADDRESS_INIT			0xC0
#define TM1637_ADDRESS_END			0xC5

/* TM1637 Display control commands (Pulse width control) */
#define TM1637_DISPLAY_PW_1_16		0x88
#define TM1637_DISPLAY_PW_2_16		0x89
#define TM1637_DISPLAY_PW_4_16		0x8A
#define TM1637_DISPLAY_PW_10_16		0x8B
#define TM1637_DISPLAY_PW_11_16		0x8C
#define TM1637_DISPLAY_PW_12_16		0x8D
#define TM1637_DISPLAY_PW_13_16		0x8E
#define TM1637_DISPLAY_PW_14_16		0x8F
#define TM1637_DISPLAY_OFF			0x80



/* Primary Functions */
void tm1637_init(void);
void tm1637_set_brightness(uint8_t pw_setting);
void tm1637_display_off(void);
void tm1637_send_digits(uint8_t *digits, uint8_t size, uint8_t dot_pos);

/* Utility functions */
void tm1637_bcd_to_2digits(uint8_t bcd, uint8_t *digit_buf, bool lead_zero);
void tm1637_num_to_4digits(uint16_t num, uint8_t *digit_buf, bool lead_zero);
void tm1637_num_to_hex(uint16_t num, uint8_t *digit_buf);


#endif /* TM1637_H_ */
