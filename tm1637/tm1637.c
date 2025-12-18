/*
 * tm1637.c
 *
 *	AVR MCU driver for TM1637 Segment LED driver
 *
 *  Created on: May 19, 2022
 *      Author: Visakhan
 */


#include <avr/io.h>
#include <util/delay.h>
#include "tm1637.h"
#include "tm1637_config.h"



/***********  MACROS *************/

#define TM1637_CLK_HIGH()		(TM1637_CLK_PORT |= (1 << TM1637_CLK_BIT))
#define TM1637_CLK_LOW()		(TM1637_CLK_PORT &= ~(1 << TM1637_CLK_BIT))
#define TM1637_DIO_HIGH()		(TM1637_DIO_PORT |= (1 << TM1637_DIO_BIT))
#define TM1637_DIO_LOW()		(TM1637_DIO_PORT &= ~(1 << TM1637_DIO_BIT))
#define TM1637_DIO_INPUT()		(TM1637_DIO_DDR &= ~(1 << TM1637_DIO_BIT))
#define TM1637_DIO_OUTPUT()		(TM1637_DIO_DDR |= (1 << TM1637_DIO_BIT))
#define TM1637_DIO_STATE()		(TM1637_DIO_PIN & (1 << TM1637_DIO_BIT))

#define TM1637_DIGIT_NULL		0




/*********** GLOBALS ************/

static uint8_t _tm1637_digits_arr[] = {
		0x3F,	/* 0 */
		0x06,	/* 1 */
		0x5B,	/* 2 */
		0x4F,	/* 3 */
		0x66,	/* 4 */
		0x6D,	/* 5 */
		0x7D,	/* 6 */
		0x07,	/* 7 */
		0x7F,	/* 8 */
		0x6F,	/* 9 */
		0x77,	/* A */
		0x7C,	/* B */
		0x39,	/* C */
		0x5E,	/* D */
		0x79,	/* E */
		0x71	/* F */
};




/************ FUNCTIONS ************/


/* Transfers one byte to TM1637 and returns ACK status */
static uint8_t send_byte(uint8_t byte);


/* Initialize AVR for communicating with TM1637 module */
void tm1637_init(void)
{

	/* Set CLK pin as output and initialize to HIGH */
	TM1637_CLK_PORT |= (1 << TM1637_CLK_BIT);
	TM1637_CLK_DDR |= (1 << TM1637_CLK_BIT);

	/* Set DIO pin as output and initialize to HIGH */
	TM1637_DIO_PORT |= (1 << TM1637_DIO_BIT);
	TM1637_DIO_DDR |= (1 << TM1637_DIO_BIT);

}


/* Send START signal to TM1637 */
static void send_start(void)
{
	/* CLK and DIO should already be HIGH here */
	//TM1637_CLK_HIGH();
	//TM1637_DIO_HIGH();
	_delay_us(3);
	TM1637_DIO_LOW();
	_delay_us(2);
}


/* Send STOP signal to TM1637 */
static void send_stop(void)
{
	/* DIO and CLK should already be LOW here */
	_delay_us(2);
	TM1637_CLK_HIGH();
	_delay_us(2);
	TM1637_DIO_HIGH();

}

/* Sends single byte to TM1637 and returns ACK status */
static uint8_t send_byte(uint8_t byte)
{
	uint8_t ack;

	/* Start sending bits LSB first */
	for(uint8_t cnt = 0; cnt < 8; cnt++) {
		TM1637_CLK_LOW();
		_delay_us(1);
		if(byte & (1 << cnt)) {
			TM1637_DIO_HIGH();
		}
		else {
			TM1637_DIO_LOW();
		}
		_delay_us(1);
		TM1637_CLK_HIGH();
		_delay_us(3);
	}

	/* ACK bit from TM1637 (9th clock) */
	TM1637_CLK_LOW();
	TM1637_DIO_LOW();
	TM1637_DIO_INPUT();	 /* Release DIO line to read ACK status */
	_delay_us(2);
	TM1637_CLK_HIGH();
	ack = TM1637_DIO_STATE();
	_delay_us(3);
	TM1637_CLK_LOW();
	TM1637_DIO_OUTPUT();  /* DIO is driven LOW */

	return ack;
}




void tm1637_set_brightness(uint8_t pw_setting)
{
	send_start();
	send_byte(pw_setting);
	send_stop();
}


void tm1637_display_off(void)
{
	send_start();
	send_byte(TM1637_DISPLAY_OFF);
	send_stop();
}


/* Sends a digits array (upto 6 digits) to display on TM1637
 * 		digits : Buffer containing digit patterns
 * 		count : Number of digits to send (up to 6)
 * 		dot_pos : Position of decimal point from rightmost digit (0 - no DP)
 *
 */
void tm1637_send_digits(uint8_t *digits, uint8_t count, uint8_t dot_pos)
{
	if((count > dot_pos) && dot_pos) {
		digits[count-dot_pos-1] |= 0x80;
	}

	send_start();
	send_byte(TM1637_WRITE_AUTOINC_CMD);
	send_stop();
	send_start();
	send_byte(TM1637_ADDRESS_INIT);
	while(count--)
	{
		send_byte(*digits++);
	}
	send_stop();
}




/* Converts a number to a 4-digit pattern
 *		num : number to be converted (numbers higher than 9999 will be truncated to 9999)
 *      digits_buf : array of at least 4 bytes to store digit pattern for the number
 *      lead_zero : true - Show leading zeroes, false - Don't show leading zeros
 */
void tm1637_num_to_4digits(uint16_t num, uint8_t *digits_buf, bool lead_zero)
{
	uint8_t d;
	uint8_t lead_z = lead_zero;

	if(num > 9999) {
		num = 9999;
	}
	d = num/1000;
	if(!d && lead_z) {
		*digits_buf++ = TM1637_DIGIT_NULL; /* Don't display leading zeros */
	}
	else {
		lead_z = 0;
		*digits_buf++ = _tm1637_digits_arr[d];
	}
	num -= d*1000;
	d = num/100;
	if(!d && lead_z) {
		*digits_buf++ = TM1637_DIGIT_NULL;
	}
	else {
		lead_z = 0;
		*digits_buf++ = _tm1637_digits_arr[d];
	}
	num -= (d*100);
	d = num/10;
	if((!d) && lead_z) {
		*digits_buf++ = TM1637_DIGIT_NULL;
	}
	else {
		lead_z = 0;
		*digits_buf++ = _tm1637_digits_arr[d];
	}
	num -= (d*10);
	d = num;
	*digits_buf = _tm1637_digits_arr[d];
}



/* Converts a number to array of hexadecimal digits */
void tm1637_num_to_hex(uint16_t num, uint8_t *digit_buf)
{
	uint8_t s = 16;
	uint8_t d;

	while(s) {
		s -= 4;
		d = (num >> s) & 0xF;
		*digit_buf++ = _tm1637_digits_arr[d];
	}
}


/* Convert a BCD byte to its 2 digit pattern */
void tm1637_bcd_to_2digits(uint8_t bcd, uint8_t *digit_buf, bool lead_zero)
{
	digit_buf[0] = 0;
	if((bcd >> 4) || lead_zero) {
		digit_buf[0] = _tm1637_digits_arr[bcd >> 4];
	}
	digit_buf[1] = _tm1637_digits_arr[bcd & 0xF];
}

