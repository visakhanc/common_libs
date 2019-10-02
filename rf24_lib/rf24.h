/**
 *	@file 	rf24.h
 *	@brief	nRF24L01/RFM7x 2.4GHz RF Module library for AVR
 *	@author	Visakhan C
 *	@date	2019/9/24
 */

#ifndef _RF24_H_
#define _RF24_H_

#include <stdint.h>

/* rf24_config.h is project specific specific and should be present in the project directory */
#include "rf24_config.h"



/**
 * \brief	The two operating modes of the RF module
 */
typedef enum {
    RF24_MODE_PTX = 0,//!< Primary Tx mode
    RF24_MODE_PRX     //!< Primary Rx mode
} rf24_opmode_t;


/**
 *	\brief 	Available Transmit power levels
 */
typedef enum {
    RF24_PWR_M18DBM = 0,//!< -18 dBm
    RF24_PWR_M12DBM,    //!< -12 dBm
    RF24_PWR_M6DBM,     //!<  -6 dBm
    RF24_PWR_0DBM       //!<   0 dBm
} rf24_power_t;

/**
 * \brief 	Available Data rates
 */
typedef enum {
	RF24_RATE_250KBPS = 0,	//!< 250 Kilobits/second
    RF24_RATE_1MBPS,   		//!< 1 Megabits/second
    RF24_RATE_2MBPS    		//!< 2 Megabits/second
} rf24_datarate_t;


/**
 *	\brief	Pipe number to identify individual pipes to be used while configuring address
 */
typedef enum {
    RF24_PIPE0 = 0,                                   //!< To identify and set address of Pipe 0
    RF24_PIPE1,                                       //!< To identify and set address of Pipe 1
    RF24_PIPE2,                                       //!< To identify and set address of Pipe 2
    RF24_PIPE3,                                       //!< To identify and set address of Pipe 3
    RF24_PIPE4,                                       //!< To identify and set address of Pipe 4
    RF24_PIPE5,                                       //!< To identify and set address of Pipe 5
	RF24_TX_ADDR                               		  //!< To set Transmit Address in PTX configuration

} rf24_pipe_t;


/**
 * \brief	Payload options used in transmit funtions
 */
typedef enum {
	RF24_TX_PLOAD, 			//!< for writing tx payload
	RF24_TX_PLOAD_NOACK, 	//!< for tx payload with no ACK
} rf24_payload_t;




/*----------------- PUBLIC FUNCTIONS -----------------*/

uint8_t rf24_init(rf24_opmode_t mode, const uint8_t *address);
void 	rf24_set_address(rf24_pipe_t pipe, const uint8_t *addr);
uint8_t rf24_transmit_packet(const uint8_t* pbuf, uint8_t len);
uint8_t rf24_transmit_packet_noack(const uint8_t *packet, uint8_t length);
uint8_t rf24_receive_packet(uint8_t* pbuf, uint8_t* length);
void 	rf24_set_ack_payload(uint8_t pipe, const uint8_t *buf, uint8_t length);
void 	rf24_powerdown(void);
void 	rf24_tx_mode(void);
void 	rf24_rx_mode(void);
void 	rf24_flush_txfifo(void);
uint8_t rf24_get_observe_tx(void);



#endif

