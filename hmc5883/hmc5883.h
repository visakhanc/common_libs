/* 
 *	HMC5883 Magnetometer library for AVR MCUs
 *	
 *	Created: Dec 19, 2017
 *	By: Visakhan C
 */

#include <stdint.h>


/************ 		MODE settings 		*********************/
#define HMC5883_MODE_SINGLE					1		/* Single measurement mode */
#define HMC5883_MODE_CONTINUOUS				0		/* Continuous measurement mode */



/************ 		CONFIG settings 		************/
/* Sampling rates */
#define HMC5883_CONFIG_RATE_0_75_HZ				(0 << 2)	/* 0.75 Hz */
#define HMC5883_CONFIG_RATE_1_5_HZ				(1 << 2)	/* 1.5 Hz */
#define HMC5883_CONFIG_RATE_3_HZ				(2 << 2)	/* 3 Hz */
#define HMC5883_CONFIG_RATE_7_5_HZ				(3 << 2)	/* 7.5 Hz */
#define HMC5883_CONFIG_RATE_15_HZ				(4 << 2)	/* 15 Hz */
#define HMC5883_CONFIG_RATE_30_HZ				(5 << 2)	/* 30 Hz */
#define HMC5883_CONFIG_RATE_75_HZ				(6 << 2)	/* 75 Hz */

/* Averaging */
#define HMC5883_CONFIG_AVG_1					(0 << 5)	/* No averaging */
#define HMC5883_CONFIG_AVG_2					(1 << 5)	/* Average 2 samples */
#define HMC5883_CONFIG_AVG_4					(2 << 5)	/* Average 4 samples */
#define HMC5883_CONFIG_AVG_8					(3 << 5)	/* Average 8 samples */

/* Measurement mode */
#define HMC5883_CONFIG_MODE_NORMAL				(0 << 0)
#define HMC5883_CONFIG_MODE_POS_BIAS			(1 << 0)
#define HMC5883_CONFIG_MODE_NEG_BIAS			(2 << 0)


/********** 		Gain Settings 		**************/
#define HMC5883_GAIN_0_88						(0 << 5)	/* Sensor Field range: +/- 0.88 Gauss */
#define HMC5883_GAIN_1_3						(1 << 5)	/* Sensor Field range: +/- 1.5 Gauss */
#define HMC5883_GAIN_1_9						(2 << 5)	/* Sensor Field range: +/- 1.9 Gauss */
#define HMC5883_GAIN_2_5						(3 << 5)	/* Sensor Field range: +/- 2.5 Gauss */
#define HMC5883_GAIN_4_0						(4 << 5)	/* Sensor Field range: +/- 4.0 Gauss */
#define HMC5883_GAIN_4_7						(5 << 5)	/* Sensor Field range: +/- 4.7 Gauss */
#define HMC5883_GAIN_5_6						(6 << 5)	/* Sensor Field range: +/- 5.6 Gauss */
#define HMC5883_GAIN_8_1						(7 << 5)	/* Sensor Field range: +/- 8.1 Gauss */

/**********  Default Values for Settings *************/
#define HMC5883_DEFAULT_CONFIG					(HMC5883_CONFIG_AVG_1|HMC5883_CONFIG_RATE_15_HZ|HMC5883_CONFIG_MODE_NORMAL)
#define HMC5883_DEFAULT_MODE					(HMC5883_MODE_SINGLE)
#define HMC5883_DEFAULT_GAIN					(HMC5883_GAIN_1_3)

/**
 * @brief Initialize HMC5338L chip with specified parameters
 * @param config : Configurations for Averaging and Output Rate
 * @param mode : Operating mode (continuous or single)
 * @param gain : Gain setting
 * @return 0 - Success
 *         1 - Error
 */
uint8_t hmc5883_init(uint8_t config, uint8_t mode, uint8_t gain);

uint8_t hmc5883_set_config(uint8_t config);
uint8_t hmc5883_set_mode(uint8_t mode);
uint8_t hmc5883_set_gain(uint8_t gain);

/**
 * @brief Read 3-axis magnetometer samples from HMC5883 chip
 * @param buf : Buffer of 6 byte size
 *        Samples read into the buffer has the format:
 *        buf[0][1] = Mag_X[15:8][7:0] - 12-bit X-axis sample in 2's complement format
 *        buf[2][3] = Mag_Z[15:8][7:0] - 12-bit Z-axis sample in 2's complement format
 *        buf[4][5] = Mag_Y[15:8][7:0] - 12-bit Y-axis sample in 2's complement format
 * @return 0 - Success
 *         1 - Error
 */
uint8_t hmc5883_get_data(uint8_t *buf);
