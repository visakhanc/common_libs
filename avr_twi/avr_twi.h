/* 
 *	AVR Two-wire serial interface 
 *
 */

#include <stdint.h>

/* Different status of TWI transfer */
typedef enum  {
	TWI_STATUS_DONE = 0,  	/* This status indicates success of a transfer */
	TWI_STATUS_BUSY,	  	/* This is the initial value of status when starting a transfer */
	TWI_STATUS_NOACK, 	  	/* This indicates error status, due to the no acknowldge */
	TWI_STATUS_ARBLOST, 	/* This indicates error status, due to arbitration loss */ 
	TWI_STATUS_BUSERROR
} twi_status_t;

/* Parameters defining a transfer, passed to the TWI transfer API */
typedef struct {
	uint8_t 		slave_addr;		/* Slave address for the transfer, should contain 7-bit slave address in bits [6:0] */
	uint8_t 		*tx_buf;		/* Buffer containing data to be transmitted to slave */
	uint8_t 		tx_count;		/* Number of bytes to be transmitted to slave */
	uint8_t 		*rx_buf;		/* Buffer to store data received from slave */
	uint8_t			rx_count;		/* Number of bytes to be received from slave */
} twi_params_t;


/* Initialize TWI bus - This will set the clock frequency to 100 kHz */
void TWI_Init(void);

/* Performs a TWI transfer, based on parameters passed and blocks until transfer is finished
 *		The twi_params_t struct should have following members initialized:
 *			slave_addr - 7 bit slave address in [6:0]
 *			tx_buf, tx_count - Valid tx buffer address and count (NULL/0 if no tx)
 *			rx_buf, rx_count - Valid rx buffer address and count (NULL/0 if no rx)
 *
 *		Returns: Final status of the transfer(0 = success)
 */
twi_status_t TWI_Master_Transfer(twi_params_t *params);


/* (NOT IMPLEMENTED)
 * Performs a Non-blocking TWI transfer, based on parameters passed.
 * Before calling, use TWI_Master_Status() to make sure TWI is currently NOT busy
 *		The twi_params_t struct should have following members initialized:
 *			slave_addr - 7 bit slave address in [6:0]
 *			tx_buf, tx_count - Valid tx buffer address and count (NULL/0 if no tx)
 *			rx_buf, rx_count - Valid rx buffer address and count (NULL/0 if no rx)
 *
 *		Returns: 0 = TWI transfer started
 *				 1 = TWI is busy and transfer is cancelled
 */
uint8_t TWI_Master_Transfer_NB(twi_params_t *params);


/*	Returns current status of TWI 
 *	Use this function with non-blocking TWI transfer to know if the previous transfer is done
 */
twi_status_t TWI_Master_Status(void);




