/*
 *  NRF24L01+ library header file
 * 	
 */
 
#ifndef NRF24L01_H
#define NRF24L01_H

#include <avr/io.h>
#include "nrf24l01_reg.h"
#include "nrf24l01_config.h"

/* Public funtions */


void nrf_init(nrf_opmode_t mode,const uint8_t *address);
uint8_t nrf_transmit_packet(uint8_t *packet, uint8_t length);
uint8_t nrf_receive_packet(uint8_t *buf, uint8_t *length);
void nrf_set_ack_payload(uint8_t pipe, uint8_t *buf, uint8_t length);

#endif  // NRF24L01_H
