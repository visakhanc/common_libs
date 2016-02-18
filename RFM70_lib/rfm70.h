/*
 *  RFM70 library header file
 * 	
 */
 
#ifndef _RFM70_H_
#define _RFM70_H_

#include <stdint.h>
#include "rfm70_config.h"
#include "rfm70_reg.h"


void rfm70_init(rfm70_opmode_t mode, const uint8_t *address);
uint8_t rfm70_transmit_packet(uint8_t* pbuf, uint8_t len);
uint8_t rfm70_receive_packet(uint8_t* pbuf, uint8_t* length);
void rfm70_set_ack_payload(uint8_t pipe, uint8_t *buf, uint8_t length);
void rfm70_powerdown(void);
void rfm70_tx_mode(void);
void rfm70_rx_mode(void);


#endif