/*
 *  ds3231.h
 *
 *  DS3231 RTC Driver for AVR
 *
 *  Created : May 21, 2022
 *  Author  : Visakhan
 */

#ifndef DS3231_H_
#define DS3231_H_

#include <stdint.h>
#include <stdbool.h>


/*********** DEFINITIONS ***********/

/* 7 bit slave address */
#define DS3231_SLA_ADDR				0x68

/* Register address and bits */
#define DS3231_ALARM1_ADDR			0x07
#define DS3231_ALARM1_SEC			0x07
#define DS3231_ALARM1_MIN			0x08
#define DS3231_ALARM1_HOUR			0x09
#define DS3231_ALARM1_DAY_DT		0x0A

#define DS3231_ALARM2_ADDR			0x0B
#define DS3231_ALARM2_MIN			0x0B
#define DS3231_ALARM2_HOUR			0x0C
#define DS3231_ALARM2_DAY_DT		0x0D

#define DS3231_CONTROL				0x0E
#define DS3231_STATUS				0x0F
#define DS3231_TEMPERATURE			0x11

#define ALARM_OFF	0
#define ALARM_ON	1


typedef enum {
	ALARM_EVERY_MINUTE,  /* At 00 second of every minute */
	ALARM_HOURLY, /* When Minutes of Alarm setting match */
	ALARM_DAILY,  /* When Hour and Minute of Alarm setting match */
	ALARM_BY_DATE, /* When Date, Hour and Minute of Alarm setting match */
	ALARM_BY_DAY  /* When Day of week, Hour and Minute of Alarm setting match */
} ds3231_alarm_rate_t;


typedef struct _ds3231_time {
	uint8_t sec;	/* Seconds in BCD format (00 - 59) */
	uint8_t min;	/* Minutes in BCD format (00 - 59) */
	uint8_t hour;	/* Hours in BCD format (Bit6=0 => 00-23 ; Bit6=1 => 1-12 (Bit5: 1=PM 0=AM) */
	uint8_t day;	/* Day of Week (1 - 7) */
	uint8_t date;	/* Date in BCD format (01 - 31) */
	uint8_t month;	/* Month in BCD format (01 - 12) */
	uint8_t year;	/* Year in BCD format (00 - 99) */
} ds3231_time_t;



typedef struct _ds3231_alarm {
	uint8_t sec;	/* Seconds in BCD format (00 - 59) */
	uint8_t min;	/* Minutes in BCD format (00 - 59) */
	uint8_t hour;	/* Hours in BCD format (Bit6=0 => 00-23 ; Bit6=1 => 1-12 (Bit5: 1=PM 0=AM) */
	uint8_t day_date; /* Date in BCD (1 - 31) or Day (1 - 7) in Bits [5:0] */
} ds3231_alarm_t;




/*********** FUNCTIONS ************/

uint8_t ds3231_init(void);
uint8_t ds3231_set_time(ds3231_time_t *time);
uint8_t ds3231_read_time(ds3231_time_t *time);
uint8_t ds3231_read_status(uint8_t *status);
uint8_t ds3231_set_dow(uint8_t dow);
uint8_t ds3231_read_alarm2(ds3231_alarm_t *alarm, bool *on);
uint8_t ds3231_set_alarm2(ds3231_alarm_t *alarm, ds3231_alarm_rate_t rate);
uint8_t ds3231_alarm2_onoff(bool on);

#endif /* DS3231_H_ */
