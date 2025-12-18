/*
 * uart_int.c
 *
 *	Interrupt driven AVR UART driver
 *
 *  Created on: Nov 15, 2025
 *      Author: Visakhan
 */

#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart_int.h"
#include "circbuf8.h"


volatile static uint8_t* _txBuf;
volatile static uint8_t _txSize;
volatile static bool _txBusy;
volatile static bool _rxOverrun;

static circbuf8_t _uart_fifo;
static uint8_t _rxBuf[64];




void uart_init(void)
{
	#include <util/setbaud.h>  // Calculate UBRR value based on BAUD value

	circbuf8_init(&_uart_fifo, _rxBuf, sizeof(_rxBuf));  // Initialize circular buffer to use with Receive interrupt

	UBRRH = UBRRH_VALUE; // Set the Baud rate with values
	UBRRL = UBRRL_VALUE; // from setbaud.h

	// Enable U2X if required - if USE_2X is defined by setbaud.h
	#if USE_2X
		UCSRA |= (1 << U2X);
	#else
		UCSRA &= ~(1 << U2X);
	#endif

	// Set USART in one stop bit,no parity,8-bit data, asynchronous mode
	UCSRC = (1<<URSEL)|DATA_8|STOP_1|PARITY_NONE;

	// Enable USART transmitter and receiver
	UCSRB = (1 << TXEN)|(1 << RXEN)|(1 << RXCIE); // Receive and Transmit enabled along with Receive interrupt

}



bool uart_send(uint8_t* txBuf, uint8_t len)
{
	if(_txBusy) {
		return false;
	}
	_txBuf = txBuf;
	_txSize = len;
	_txBusy = true;
	UDR = _txBuf[0];
	UCSRB |= (1 << UDRIE);  // Enable UDRE interrupt

	return true;
}


bool uart_busy(void)
{
	return _txBusy;
}


void uart_PutString(char* str, uint8_t len)
{
	while(_txBusy)
		;
	uart_send((uint8_t *)str, len);
}



uint8_t uart_receive(uint8_t* rxBuf, uint8_t len)
{
	return circbuf8_read_buf(&_uart_fifo, rxBuf, len);
}


uint8_t uart_remaining(void)
{
	return circbuf8_count(&_uart_fifo);

}

bool uart_overrun(void)
{
	return _rxOverrun;
}



/* Receive Complete ISR */
ISR(USART_RXC_vect)
{
	_rxOverrun = circbuf8_write(&_uart_fifo, UDR);
}


/* UDR Empty (UDRE) Interrupt */
ISR(USART_UDRE_vect)
{
	static uint8_t i = 0;

	i++;
	if(i < _txSize) {
		UDR = _txBuf[i];
	}
	else {
		i = 0;
		_txBusy = false;
		UCSRB &= ~(1 << UDRIE); // End of transmission - disable UDRE interrupt (otherwise ISR will be executed forever)
	}
}

