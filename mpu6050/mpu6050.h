/* 
 *	MPU6050 Accelerometer-Gyroscope sensor library for AVR MCUs
 *	
 *	Created: May 10, 2016
 */

#include <stdint.h>

/* Different operating modes */
#define MPU6050_NORMAL_MODE				0		/* All six DOF sensors ON */
#define MPU6050_LOW_POWER_ACCEL_MODE	1		/* Low power mode with only Accelerometer sampling */


/* Initializes MPU6050 module with specified 'mode'
 * modes:
 *     MPU6050_NORMAL_MODE: Accelerometer and Gyroscope are enabled
 *     MPU6050_LOW_POWER_ACCEL_MODE: Only accelerometer is enabled
 * Returns:
 * 	   0 - success
 * 	   1 - Error
 */
uint8_t mpu6050_init(uint8_t mode);


/*
 * Get data from MPU6050 sensor data registers
 * Data is read starting from address 0x3B(59) onwards. The data are as follows:
 *     [0x3B,0x3C], [0x3D,0x3E], [0x3F,0x40] : Accelerometer X[15:8][7:0], Y[15:8][7:0], Z[15:8][7:0]
 *     [0x41,0x42]                           : Temperature [15:8][7:0]
 *     [0x43,0x44], [0x45,0x46], [0x47,0x48] : Gyroscope X[15:8][7:0], Y[15:8][7:0], Z[15:8][7:0]
 * Arguments:
 * buf - Location of buffer to store read data
 * count - Number of bytes to read (Upto 14 bytes can be read)
 *
 * Returns:
 *     0 - Success
 *     1 - Error
 */
uint8_t mpu6050_get_data(uint8_t *buf, uint8_t count);

