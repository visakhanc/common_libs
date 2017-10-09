/* 
 *  Library to detect IR remote control code pulses through TSOP18xx IR receiver
 *		- INT0 interrupt and Timer0 is used
 *
 *  TSOP1838 output pin is connected to INT0 of AVR
 * 	Timer0 is used for timing
 *
 *	This library is for Sony IR protocol (15-bit)
 * 	
 */
 
#include <stdint.h>

#include "ir_config.h"

/* 	
 * KEY-CODES FOR 'data' of Sony Car stereo Remote (Command = 0x84)
 */

#define RC_ONE 			0x00
#define RC_TWO 			0x01
#define RC_THREE 		0x02
#define RC_FOUR 		0x03
#define RC_FIVE 		0x04
#define RC_SIX 			0x05
#define RC_SEVEN 		0x06
#define RC_EIGHT 		0x07
#define RC_NINE 		0x08
#define RC_TEN			0x09

#define RC_LEFT			0x35
#define RC_RIGHT		0x34
#define RC_UP			0x33
#define RC_DOWN			0x32
#define RC_OK			0x5C /* ENTER */
#define RC_VOL_UP 		0x12
#define RC_VOL_DOWN 	0x13

#define RC_POWER		0x0D
#define RC_MUTE			0x14 /* ATT */
#define RC_SOURCE		0x46
#define RC_MENU			0x10
#define RC_RETURN		0x0B
#define RC_MODE			0x47
#define RC_SEARCH		0x27 /* Lens */

typedef struct _rc_code {
	uint8_t addr;  	/* 8-bit */
	uint8_t data; 	/* 7 bit */
} rc_code_t;


void rc_init(void);
uint8_t rc_wait_get(rc_code_t *code);
uint8_t rc_get(rc_code_t *code);
