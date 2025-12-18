/*
 *  ds3231.c
 *
 *  DS3231 RTC Driver for AVR
 *
 *  Created: May 21, 2022
 *  Author:  Visakhan
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "avr_twi.h"
#include "ds3231.h"


static twi_params_t _ds3231_params = { .slave_addr = DS3231_SLA_ADDR};



static uint8_t ds3231_read_bytes(uint8_t addr, uint8_t *buf, uint8_t count)
{
	uint8_t tx_byte = addr;

	_ds3231_params.tx_buf = &tx_byte;
	_ds3231_params.tx_count = 1;
	_ds3231_params.rx_buf = buf;
	_ds3231_params.rx_count = count;

	return TWI_Master_Transfer(&_ds3231_params);
}

/* buf[0] should contain the register address of write */
static uint8_t ds3231_write_bytes(uint8_t *buf, uint8_t count)
{
	_ds3231_params.tx_buf = buf;
	_ds3231_params.tx_count = count;
	_ds3231_params.rx_count = 0;

	return TWI_Master_Transfer(&_ds3231_params);
}






uint8_t ds3231_init(void)
{
	uint8_t ret;
	uint8_t buf[5] = {DS3231_ALARM1_ADDR, 0x80, 0x80, 0x80, 0x81};

	/* Initialize AVR I2C bus */
	TWI_Init();

	ret = ds3231_write_bytes(buf, sizeof(buf));
	if(ret) {
		return ret;
	}

	buf[0] = DS3231_STATUS;
	//buf[1] = 0x05; /* INTCN = 1, A1IE = 1 -> Enable Alarm 1 interrupt */
	buf[1] = 0x00; /* Clear flags in STATUS register and disable 32kHz output*/
	buf[2] = 0x00; /* Clear Aging offset register */
	return ds3231_write_bytes(buf, 3);


//	buf[0] = DS3231_CONTROL;
//	buf[1] = 0; /* INTCN = 0, RS[2:1] = 0 -> 1Hz Square wave output on INT/SQW pin */
//	return ds3231_write_bytes(buf, 2);

}


uint8_t ds3231_set_dow(uint8_t dow)
{
	uint8_t buf[2];

	buf[0] = 0x03;
	buf[1] = dow;
	return ds3231_write_bytes(buf, sizeof(buf));
}

uint8_t ds3231_read_time(ds3231_time_t *rtc_time)
{
	uint8_t ret;

	ret = ds3231_read_bytes(0, (uint8_t *)rtc_time, sizeof(*rtc_time));
	if(rtc_time->hour & (1 << 6)) {
		rtc_time->hour &= 0x1F;
	}
	return ret;
}


uint8_t ds3231_set_time(ds3231_time_t *rtc_time)
{
	uint8_t	buf[sizeof(*rtc_time) + 1];
	uint8_t *ptr = (uint8_t *)rtc_time;
	uint8_t i = 0;

	buf[i++] = 0;	/* Write reg address = 0 */
	while(i < sizeof(buf)) {
		buf[i++] = *ptr++;
	}
	return ds3231_write_bytes(buf, sizeof(buf));
}



uint8_t ds3231_read_alarm2(ds3231_alarm_t *alarm, bool *on)
{
	uint8_t ret;
	uint8_t buf[4];

	ret = ds3231_read_bytes(DS3231_ALARM2_ADDR, buf, sizeof(buf));
	alarm->min = buf[0] & 0x7F;
	alarm->hour = buf[1] & 0x7F;
	alarm->day_date = buf[2] & 0x3F;
	*on = (buf[3] & 0x02) ? true : false;

	return ret;
}



uint8_t ds3231_set_alarm2(ds3231_alarm_t *alarm, ds3231_alarm_rate_t rate)
{
	uint8_t	buf[4];

	switch(rate) {
	case ALARM_EVERY_MINUTE: alarm->min |= 0x80;
	case ALARM_HOURLY: alarm->hour |= 0x80;
	case ALARM_DAILY: alarm->day_date |= 0x80;
	case ALARM_BY_DAY: alarm->day_date |= 0x40;
	}
	buf[0] = DS3231_ALARM2_ADDR;
	buf[1] = alarm->min;
	buf[2] = alarm->hour;
	buf[3] = alarm->day_date;
	return ds3231_write_bytes(buf, sizeof(buf));
}


uint8_t ds3231_alarm2_onoff(bool on)
{
	uint8_t ret;
	uint8_t buf[2];

	buf[0] = DS3231_CONTROL;
	buf[1] = (on) ? 0x07 : 0x05;
	return ds3231_write_bytes(buf, sizeof(buf));  /* Set/Clear A2IE bit of CONTROL register */
}



uint8_t ds3231_read_status(uint8_t *status)
{
	uint8_t ret;
	uint8_t buf[2] = {DS3231_STATUS, 0x00};

	ret = ds3231_read_bytes(DS3231_STATUS, status, 1);  /* Read Status register */
	if(!ret) {
		ret = ds3231_write_bytes(buf, sizeof(buf));  /* Clear flags */
	}
	return ret;
}


