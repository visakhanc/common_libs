/*
 * uart_int.h
 *	
 *	Interrupt driven AVR UART driver
 *
 *  Created on: Nov 15, 2025
 *      Author: Visakhan
 */

#ifndef UART_INT_H_
#define UART_INT_H_

#include <stdbool.h>
#include <avr/io.h>

/*********** MACROS ****************/

#define DATA_8		(3 << UCSZ0)
#define DATA_7 		(2 << UCSZ0)
#define DATA_6 		(1 << UCSZ0)
#define DATA_5 		(0 << UCSZ0)

#define STOP_2 		(1<<USBS)
#define STOP_1 		(0<<USBS)
#define PARITY_NONE	0
#define PARITY_EVEN (2<<UPM0)
#define PARITY_ODD 	(3<<UPM0)



/************ FUNCTIONS ****************/

/* Initialize the UART with 8N1 format
 * Baud rate BAUD should be defined in the board configuration header file
 */
void uart_init(void);




/* Returns: true - txBuf queued for sending
 * 			false - Not sent due to ongoing transmission
 */
bool uart_send(uint8_t* txBuf, uint8_t len);



/* Returns: true -  UART is busy due to ongoing transmission
 * 			false - UART is ready to send data
 */
bool uart_busy(void);




/* Waits for any ongoing transmission to be over and queues the specified length of string for sending */
void uart_PutString(char* str, uint8_t len);




/* Returns : Number of bytes read into rxBuf (0 if empty Receive FIFO)
 */
uint8_t uart_receive(uint8_t* rxBuf, uint8_t len);




/* Returns : Number of bytes remaining to be read in Receive FIFO */
uint8_t uart_remaining(void);




/* Returns: true - Receive FIFO is full and last received character is lost
			false - Receive FIFO is not full */
bool uart_overrun(void);





#endif /* UART_INT_H_ */
