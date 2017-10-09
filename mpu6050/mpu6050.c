
#include "mpu6050.h"
#include "avr_twi.h"

#define SLA_ADDR	0x68

/* Initialize MPU6050
 * 	- with internal oscillator
 *  - sampling rate = 10 Hz
 *  - interrupt on new sample
 *  - disable sleep mode
 */
uint8_t mpu6050_init(uint8_t mode)
{
	twi_params_t  	params = {0};
	uint8_t 	 	tx_data[3];
	
	/* Initialize AVR TWI bus */
	TWI_Init();
	
	params.slave_addr = SLA_ADDR;
	/* Disable sleep mode */
	tx_data[0] = 0x6B; /* PWR_MGMT_1 reg */
	tx_data[1] = (1 << 3);
	params.tx_buf = &tx_data[0];
	params.tx_count = 2;
	if(TWI_Master_Transfer(&params) != TWI_STATUS_DONE) {
		return 1;
	}
	if(mode == MPU6050_LOW_POWER_ACCEL_MODE) {
		/* Enable Low power Accelerometer-only mode */
		tx_data[0] = 0x6B;
		tx_data[1] = (1 << 3)|(1 << 5);
		tx_data[2] = 0x7;
		params.tx_buf = &tx_data[0];
		params.tx_count = 3;
		if(TWI_Master_Transfer(&params) != TWI_STATUS_DONE) {
			return 1;
		}
		/* Change wakeup rate to 40Hz */
		tx_data[0] = 0x6C;
		tx_data[1] = (1 << 7)|(1 << 6)|0x7;
		params.tx_buf = &tx_data[0];
		params.tx_count = 2;
		if(TWI_Master_Transfer(&params) != TWI_STATUS_DONE) {
			return 1;
		}
	}
	/* Set accelerometer Full Scale range to +/-4g */
	tx_data[0] = 0x1C;
	tx_data[1] = (1 << 3);
	params.tx_buf = &tx_data[0];
	params.tx_count = 2;
	if(TWI_Master_Transfer(&params) != TWI_STATUS_DONE) {
		return 1;
	}
	return 0;
}


uint8_t mpu6050_get_data(uint8_t *buf, uint8_t count)
{
	twi_params_t  	params = {0};
	uint8_t 		tx_data = 0x3B;
	
	params.slave_addr = SLA_ADDR;
	params.tx_buf = &tx_data;
	params.tx_count = 1;
	params.rx_buf = buf;
	params.rx_count = count;
	
	if(TWI_Master_Transfer(&params) != TWI_STATUS_DONE) {
		return 1;
	}
	return 0;
}
	
