/*
 * circbuf8.h
 * 
 * Implements a Circular buffer with a maximum size of 255 bytes 
 *
 * Created on: Dec 7, 2017
 *      Author: Visakhan C
 */

#ifndef CIRCBUF8_H
#define CIRCBUF8_H

#include <stdbool.h>
#include <stdint.h>


typedef struct circbuf8_handle {
	volatile uint8_t w_index;
	volatile uint8_t r_index;
	uint8_t size;
	uint8_t *buf;
} circbuf8_t;



/* Initialize Circular buffer
 *
 * Parameters:
 * handle : pointer to a circbuf_t structure
 * buf : buffer location
 * size : size of buffer
 *
 * Note: The circular buffer uses only (size - 1) bytes of the buffer
 */
void circbuf8_init(circbuf8_t *handle, uint8_t *buf, uint8_t size);


/* Write a byte to Circular buffer 
 * 	Returns 0: success 
			1: Buffer full (Data not written)
 */
uint8_t circbuf8_write(circbuf8_t *handle, uint8_t data);



/* Read a byte from Circular buffer 
 *
 * Returns 	0: Success 
 *			1: No data to read
 */
uint8_t circbuf8_read(circbuf8_t *handle, uint8_t *data);



/* Writes a buffer of bytes to Circular buffer
 * Note: Maximum 255 bytes can be written at a time
 *
 * Returns 	0 : Success
 *			>0 : Number of empty bytes in the circular buffer (Data buffer is not written)
 */
uint8_t circbuf8_write_buf(circbuf8_t *handle, uint8_t *buf, uint8_t len);



/* Reads multiple bytes from Circular buffer
 *
 * 'len' number of bytes will be read into buffer location 'buf'
 * Note: Upto 255 bytes can be read
 * 
 * Returns: Number of bytes read (can be less than 'len')
 */
uint8_t circbuf8_read_buf(circbuf8_t *handle, uint8_t *buf, uint8_t len);



/* Returns the number of unread bytes in Circular buffer */
uint8_t circbuf8_count(circbuf8_t *handle);

#endif
