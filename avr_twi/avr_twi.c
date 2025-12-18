#include <avr/io.h>
#include <util/twi.h>
#include <avr/interrupt.h>
#include "avr_twi.h"

//#define TWI_DEBUG

#define RED_LED					PC1
#define RED_LED_DDR				DDRC
#define RED_LED_PORT			PORTC

#ifdef TWI_DEBUG
	#define RED_LED_OUT()			(RED_LED_DDR |= (1 << RED_LED))
	#define RED_LED_OFF()			(RED_LED_PORT |= (1 << RED_LED))
	#define RED_LED_ON()			(RED_LED_PORT &= ~(1 << RED_LED))
	#define RED_LED_TOGGLE()		(RED_LED_PORT ^= (1 << RED_LED))
#else
	#define RED_LED_OUT()
	#define RED_LED_ON()
	#define RED_LED_OFF()
	#define RED_LED_TOGGLE()
#endif

static volatile uint8_t  twi_status = TWI_STATUS_DONE;
static twi_params_t		_twi_params; /* Local structure to copy the parameters from caller */


/* Initialize TWI module */
void TWI_Init(void)
{
	/* TWBR setting for 400kHz SCL clock */
	RED_LED_OUT();
#if F_CPU == 20000000
	TWBR = 17; // 20MHz crystal => 408kHz
#elif F_CPU == 16000000
	TWBR = 12; // 16MHz crystal => 410kHz
#elif F_CPU == 12000000
	TWBR = 7; // 12MHz crystal => 413kHz
#elif F_CPU == 8000000
	TWBR = 2; // 8MHz (crystal/internal RC) =>  421kHz
#elif F_CPU == 4000000
	TWBR = 2; // 4MHz => 210kHz
#elif F_CPU == 2000000
	TWBR = 2; // 2MHz => 105kHz
#elif F_CPU ==  1000000
	TWBR = 2; // 1MHz => 52kHz
#else
	#error "F_CPU value not OK for TWI driver. Please change F_CPU or add to TWI driver"
#endif
}

/* TWI transfer API */
twi_status_t TWI_Master_Transfer(twi_params_t *params)
{
	/* Previous transfer complete ? */
	if(twi_status == TWI_STATUS_BUSY) {
		return TWI_STATUS_BUSY;
	}
	
	/* Set parameters for the current transfer */
	twi_status = TWI_STATUS_BUSY;
	_twi_params = *params;
	RED_LED_ON();
	/* Send start condition */
	TWCR = _BV(TWINT)|_BV(TWEA)|_BV(TWSTA)|_BV(TWEN)|_BV(TWIE); // Enable interrupt as well 
	
	while(twi_status == TWI_STATUS_BUSY)
		;
	RED_LED_OFF();
	return twi_status;
}


/* TWI Non-blocking transfer API (NOT IMPLEMENTED) */
uint8_t TWI_Master_Transfer_NB(twi_params_t *params)
{
	/* Previous transfer complete ? */
	if(twi_status == TWI_STATUS_BUSY) {
		return 1;
	}
	
	/* Set parameters for the current transfer */
	twi_status = TWI_STATUS_BUSY;
	_twi_params = *params;
	RED_LED_ON();
	/* Send start condition */
	TWCR = _BV(TWINT)|_BV(TWEA)|_BV(TWSTA)|_BV(TWEN)|_BV(TWIE); // Enable interrupt as well 
	return 0;
}


twi_status_t TWI_Master_Status(void)
{
	return twi_status;
}


/* ISR of TWI interrupt */
ISR(TWI_vect)
{
	uint8_t 	twst;
	
	/* Check TWI status */
	switch(twst = TW_STATUS) {
		case TW_START:  /* start condition is transmitted, now transmit SLA+R/W */
		case TW_REP_START : /* Repeated start: transmit SLA+R/W */
			TWDR = (_twi_params.slave_addr << 1) | (_twi_params.tx_count == 0);
			TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWIE);
			break;
			
		case TW_MT_SLA_ACK: /* SLA+W / data has been transmitted and ACK received : Transmit further data or send START or STOP */
		case TW_MT_DATA_ACK:
			if(_twi_params.tx_count) { /* Transmit remaining byte(s) */
				TWDR = *_twi_params.tx_buf++;
				_twi_params.tx_count--;
				TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWIE);
			}
			else {
				if(_twi_params.rx_count) {	
					TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWIE)|_BV(TWSTA); /* send START if data to be read */ 
				}
				else {
					TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWIE)|_BV(TWSTO); /* otherwise STOP to finish transfer */
					twi_status = TWI_STATUS_DONE;
				}
			}
			break;
			
		case TW_MR_SLA_ACK: /* SLA+R has been transmitted and ACK received : Now receive data byte(s) with ACK or NACK */
			if(_twi_params.rx_count == 1) {
				TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWIE);  /* Dont ACK if last byte to be received */
			}
			else {
				TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWIE)|_BV(TWEA); /* Receive data with ACK */
			}
			break;
			
		case TW_MR_DATA_ACK: /* Data byte received and ACK sent: Receive next byte(s) with ACK/NACK */
			if(_twi_params.rx_count == 1) {
				TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWIE);  /* Dont ACK if last byte to be received */
			}
			else {
				TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWIE)|_BV(TWEA); /* Receive data with ACK */
			}
			*_twi_params.rx_buf++ = TWDR;
			_twi_params.rx_count--;
			break;
			
		case TW_MR_DATA_NACK: /* Data byte received and no ACK sent: Send STOP to finish transfer */
			TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWIE)|_BV(TWSTO);
			twi_status = TWI_STATUS_DONE;
			break;
			
		/* NACK conditions */
		case TW_MR_SLA_NACK:  /* SLA+R transmitted, NACK received */
		case TW_MT_SLA_NACK:  /* SLA+W transmitted, ACK received */
		case TW_MT_DATA_NACK: /* data transmitted, NACK received */
			TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWIE)|_BV(TWSTO);
			twi_status = TWI_STATUS_NOACK;
			break;
			
		case TW_MT_ARB_LOST:  /* arbitration lost in SLA+W or data : Release TWI bus*/
			twi_status = TWI_STATUS_ARBLOST;
			TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWIE);  
			break;
		case TW_BUS_ERROR: /* Bus error : send STOP */
			twi_status = TWI_STATUS_BUSERROR;
			TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWIE)|_BV(TWSTO);
			break;
	}
	
}



void TWI_Reset(void)
{
    TWCR &= ~(_BV(TWSTO)|_BV(TWEN));
    TWCR |= _BV(TWEN);
}
