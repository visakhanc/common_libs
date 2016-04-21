/* 
 *  Library to detect remote control code pulses through TSOP18xx IR receiver
 *		- INT0 interrupt and Timer0 is used
 */
 
#include <stdint.h>


/* 	KEY-CODES FOR PANASONIC LCD TV REMOTE
	Assumes 2 byte address is 0x4004, as found out
	Assumes higher 2 byte data is 0x0010 as found out
	These values are lower 2 bytes of data 
*/

#define RC_ONE 		0x0809
#define RC_TWO 		0x8889
#define RC_THREE 	0x4849
#define RC_FOUR 	0xC8C9
#define RC_FIVE 	0x2829
#define RC_SIX 		0xA8A9
#define RC_SEVEN 	0x6869
#define RC_EIGHT 	0xE8E9
#define RC_NINE 	0x1819
#define RC_ZERO		0x9899

#define RC_RED		0x0E0F
#define RC_GREEN	0x8E8F
#define RC_YELLOW	0x4E4F
#define	RC_BLUE		0xCECF

#define RC_LEFT		0x7273
#define RC_RIGHT	0xF2F3
#define RC_UP		0x5253
#define RC_DOWN		0xD2D3
#define RC_OK		0x9293

#define RC_VOL_UP 	0x0405
#define RC_VOL_DOWN 0x8485
#define RC_CH_UP 	0x2C2D
#define RC_CH_DOWN	0xACAD
#define RC_MENU		0x4A4B
#define RC_EXIT		0xCBCA
#define RC_RETURN	0x2B2A


typedef struct _rc_code {
	uint8_t addr[2];
	uint8_t data[4];
} rc_code_t;


void rc_init(void);
uint8_t rc_get_code(rc_code_t *code);
