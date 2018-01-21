/* 
 *	HMC5883 Magnetometer library for AVR MCUs
 *	
 *	Created: Dec 19, 2017
 *	By: Visakhan C
 */

#include "hmc5883.h"
#include "avr_twi.h"

#define HMC5883_SLA_ADDR	0x1E

uint8_t hmc5883_set_config(uint8_t config);
uint8_t hmc5883_set_mode(uint8_t mode);
uint8_t hmc5883_set_gain(uint8_t gain);
uint8_t hmc5883_get_data(uint8_t *buf);

/* Initialize HMC5883 IC */
uint8_t hmc5883_init(uint8_t config, uint8_t mode, uint8_t gain)
{
	twi_params_t  	params = {0};
	uint8_t 	 	tx_data[4];
	
	/* Initialize AVR TWI bus */
	TWI_Init();
	
	params.slave_addr = HMC5883_SLA_ADDR;
	/* Disable sleep mode */
	tx_data[0] = 0x0; /* Start from register 0 */
	tx_data[1] = config; /* Config Reg A */
	tx_data[2] = gain; /* Config Reg B */
	tx_data[3] = mode; /* Mode Reg */
	params.tx_buf = &tx_data[0];
	params.tx_count = 4;
	if(TWI_Master_Transfer(&params) != TWI_STATUS_DONE) {
		return 1;
	}
	return 0;
}


uint8_t hmc5883_get_data(uint8_t *buf)
{
	twi_params_t  	params = {0};
	uint8_t 		tx_data = 0x3;	/* Data registers start at 0x3 */
	
	params.slave_addr = HMC5883_SLA_ADDR;
	params.tx_buf = &tx_data;
	params.tx_count = 1;
	params.rx_buf = buf;
	params.rx_count = 6;	/* Read all 6 registers(8-bit) */
	
	if(TWI_Master_Transfer(&params) != TWI_STATUS_DONE) {
		return 1;
	}
	return 0;
}
	

uint8_t hmc5883_set_config(uint8_t config)
{

}


uint8_t hmc5883_set_mode(uint8_t mode)
{

}


uint8_t hmc5883_set_gain(uint8_t gain)
{

}
