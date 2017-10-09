/* 
 *  Library to detect RC5 code pulses through TSOP1768 IR receiver
 *		TSOP1838 output pin is connected to INT0/INT1 of AVR - Define this pin in ir_config.h
 */
 
#include <stdint.h>

#include "ir_config.h"

// KEY-CODES FOR PHILIPS TV REMOTE
#define RC5_ZERO 0
#define RC5_ONE 1
#define RC5_TWO 2
#define RC5_THREE 3
#define RC5_FOUR 4
#define RC5_FIVE 5
#define RC5_SIX 6
#define RC5_SEVEN 7
#define RC5_EIGHT 8
#define RC5_NINE 9

#define RC5_VOL_UP 16
#define RC5_VOL_DOWN 17
#define RC5_CH_UP 32
#define RC5_CH_DOWN 33
#define RC5_MENU 18
#define RC5_OK 23
#define RC5_UP 16
#define RC5_DOWN 17
#define RC5_RIGHT 22
#define RC5_LEFT 21

#define RC5_POWER 12
#define RC5_MUTE 13
#define RC5_RED 43
#define RC5_GREEN 44
#define RC5_YELLOW 45
#define RC5_BLUE 46

#define RC5_ERROR 0xFF



typedef struct _rc5_code {
	uint8_t addr;
	uint8_t data;
} rc5_code_t;


void rc5_init(void);
uint8_t rc5_get_code(rc5_code_t *code);
