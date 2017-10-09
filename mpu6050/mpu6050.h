#include <stdint.h>

/* Different operating modes */
#define MPU6050_NORMAL_MODE				0		/* All six DOF sensors ON */
#define MPU6050_LOW_POWER_ACCEL_MODE	1		/* Low power mode with only Accelerometer sampling */

uint8_t mpu6050_init(uint8_t mode);
uint8_t mpu6050_get_data(uint8_t *buf, uint8_t count);

