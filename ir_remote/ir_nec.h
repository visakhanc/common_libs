/* 
 *  Library to detect IR remote control code pulses through TSOP18xx IR receiver
 *
 *  TSOP1838 output pin is connected to INT0/INT1 of AVR - Define this pin in ir_config.h
 * 	Timer0 is used for timing
 *
 *	This library is for IR NEC protocol
 * 	TODO: Protocol uses LSB-first, so we need to bit reverse the received code 
 */
 
#include <stdint.h>

#include "ir_config.h"

/* 	
 * KEY-CODES FOR 'data' of DVD REMOTE (NEC coding)
 */

#define RC_ONE 			0xB0
#define RC_TWO 			0x90
#define RC_THREE 		0xA0
#define RC_FOUR 		0xF2
#define RC_FIVE 		0xD2
#define RC_SIX 			0xE2
#define RC_SEVEN 		0x72
#define RC_EIGHT 		0x52
#define RC_NINE 		0x62
#define RC_ZERO			0xB2

#define RC_LEFT			0x32
#define RC_RIGHT		0x02
#define RC_UP			0x22
#define RC_DOWN			0x12
#define RC_OK			0x60
#define RC_VOL_UP 		0x18
#define RC_VOL_DOWN 	0x28

#define RC_PLAY			0x10
#define RC_STOP			0x50
#define RC_PAUSE		0x20
#define RC_MUTE			0xFA
#define RC_PREV			0xDA
#define RC_NEXT			0x38
#define RC_FASTFWD		0x08
#define RC_REWIND		0xEA

#define RC_MENU			0x4A
#define RC_RETURN		0xBA
#define RC_POWER		0x30
#define RC_EJECT		0x00
#define RC_SETUP		0xF0
#define RC_SEARCH		0xC0
#define RC_CLEAR		0xA2
#define RC_STEP			0xD0
#define RC_TITLE		0x8A
#define RC_ZOOM			0x2A
#define RC_REPEAT		0xAA
#define RC_SOURCE		0x7A

typedef struct _rc_code {
	uint8_t addr;
	uint8_t data;
} rc_code_t;


void rc_init(void);
uint8_t rc_wait_get(rc_code_t* code);
uint8_t rc_get(rc_code_t* code);

