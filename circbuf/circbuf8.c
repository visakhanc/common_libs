/*
 * circbuf8.c
 *
 * Implements a Circular buffer with a maximum size of 255 bytes 
 *
 * Created on: Dec 7, 2017
 * Author: Visakhan C
 */

#include "circbuf8.h"


/* Initialize Circular buffer
 *
 * 'buf' points to the location of a buffer of 'size' bytes
 * The circular buffer uses only (size - 1) bytes of this buffer
 */
void circbuf8_init(circbuf8_t *handle, uint8_t *buf, uint8_t size)
{	
	handle->buf = buf;
	handle->size = size;
	handle->r_index = 0;
	handle->w_index = 0;
}

/* Write a byte to Circular buffer 
 *
 * Write to current location only if buffer is not full.  
 * Then, increment pointer if next location is not read pointer
 * 	
 * Returns 	0: Success
 * 			1: Buffer full (Data not written)
 */
uint8_t circbuf8_write(circbuf8_t *handle, uint8_t data)
{
	/* Calculate Next write index */
	uint8_t next = handle->w_index + 1;
	if(next == handle->size) {
		next = 0;
	}
	/* If buffer is full, return error */
	if(next == handle->r_index) { 
		return 1;
	}
	/* Write data to buffer */
	handle->buf[handle->w_index] = data;
	/* Update write index */
	handle->w_index = next;
	return 0; 
}


/* Read a byte from Circular buffer 
 *
 * Reads byte and increment pointer
 * If no data to read, returns error
 *  
 * Returns 	0: Success 
 *			1: No data to read
 */
uint8_t circbuf8_read(circbuf8_t *handle, uint8_t *data)
{
	/* If no data to read, return */
	if(handle->r_index == handle->w_index) {
		return 1;
	}
	*data = handle->buf[handle->r_index];
	handle->r_index++;
	if(handle->r_index == handle->size) {
		handle->r_index = 0;
	}
	return 0;
}



/* Writes a buffer of bytes to Circular buffer
 *
 * Note: Maximum 255 bytes can be written at a time
 *
 * Returns 	0 : Success
 *			>0 : Number of empty bytes in the circular buffer (Data buffer is not written)
 */
uint8_t circbuf8_write_buf(circbuf8_t *handle, uint8_t *buf, uint8_t len)
{
	uint8_t n = 0;
	uint8_t remain = handle->size - circbuf8_count(handle) - 1;
	
	/* If no space to write buffer, return */
	if(remain < len) {
		return remain;
	}
	/* Write buffer */
	do {
		handle->buf[handle->w_index] = *buf++;
		handle->w_index++;
		if(handle->w_index == handle->size) {
			handle->w_index = 0;
		}
		n++;
	} while(n < len);
	return 0;
}


/* Reads multiple bytes from Circular buffer
 *
 * 'len' number of bytes will be read into buffer location 'buf'
 * Note: Upto 255 bytes can be read
 * 
 * Returns: Number of bytes read (can be less than 'len')
 */
uint8_t circbuf8_read_buf(circbuf8_t *handle, uint8_t *buf, uint8_t len)
{
	uint8_t n = 0;
	
	do {
		if(handle->r_index == handle->w_index) {
			break;
		}
		*buf++ = handle->buf[handle->r_index];
		handle->r_index++;
		if(handle->r_index == handle->size) {
			handle->r_index = 0;
		}
		n++;
	} while(n < len);
	return n;
}


/* Returns the number of unread bytes in Circular buffer 
 */
uint8_t circbuf8_count(circbuf8_t *handle)
{
	uint8_t r = handle->r_index;
	uint8_t w = handle->w_index;
	return (r <= w) ? (w - r) : (handle->size - (r - w));
}
