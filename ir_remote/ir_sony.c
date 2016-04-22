/* 
 *  Library to detect IR remote control code pulses through TSOP1838 IR receiver
 *		- INT0 interrupt and Timer0 is used
 *	TSOP1838 output pin is connected to INT0 of AVR
 * 	Timer0 is used for timing
 *
 *	This library is for Sony IR (SIRC?) protocol
 * 	TODO: Protocol uses LSB-first, so we need to bit reverse the received code 
 */
 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "ir_sony.h"

#if F_CPU == 1000000
	#define TCCR0_VAL 			3	 /* 64 us */
	#define HEADER_LOW_COUNT	32   /* For 64us timer, 2ms ~= 32 counts*/
	#define PULSE_THRESHOLD		15   /* 1ms, low pulse threshold */
	#define IDLE_HIGH_COUNT		1	 /* 20 ms, timer0_ovf count */
#elif F_CPU == 2000000
	#define TCCR0_VAL 			3	 /* 32 us */
	#define HEADER_LOW_COUNT	64   /* For 32us timer, 2ms ~= 64 counts*/
	#define PULSE_THRESHOLD		30   /* 1ms, low pulse threshold */
	#define IDLE_HIGH_COUNT		2	 /* 20 ms, timer0_ovf count */
#elif F_CPU == 4000000
	#define TCCR0_VAL 			4 	/* 64 us */
	#define HEADER_LOW_COUNT	32   /* For 64us timer, 2ms ~= 32 counts*/
	#define PULSE_THRESHOLD		15   /* 1ms, low pulse threshold */
	#define IDLE_HIGH_COUNT		1	 /* 20 ms, timer0_ovf count */
#elif F_CPU == 8000000
	#define TCCR0_VAL 			4	 /* 32 us */
	#define HEADER_LOW_COUNT	64   /* For 32us timer, 2ms ~= 64 counts*/
	#define PULSE_THRESHOLD		30   /* 1ms, low pulse threshold */
	#define IDLE_HIGH_COUNT		2	 /* 20 ms, timer0_ovf count */
#elif F_CPU == 12000000
	#define TCCR0_VAL 			5	 /* 85 us */
	#define HEADER_LOW_COUNT	23   /* For 85us timer, 2ms ~= 23 counts*/
	#define PULSE_THRESHOLD		11   /* 1ms, low pulse threshold */
	#define IDLE_HIGH_COUNT		1	 /* 20 ms, timer0_ovf count */
#else
	#error F_CPU not supported for ir_panasonic.c, use another F_CPU
#endif

enum rc_state {
	RC_INIT = 0,
	RC_START,
	RC_SAMP
};

static uint8_t rx_code[2];
static uint8_t timer0_ovf;
static uint8_t edge;
static enum rc_state state; 
static volatile uint8_t ready = 0;

static void Timer0_Init(void)
{
	TIMSK |= (1 << 0);  // Enable Timer0 Overflow interrupt

	/* Start Timer0 */
	TCCR0 = TCCR0_VAL;
}

void rc_init(void)
{
	Timer0_Init();
	
	/* External INT0 */
	MCUCR |= (1 << 1); // Falling edge generates interrupt
	MCUCR &= ~(1 << 0);
	GICR |= (1 << INT0); // Enable interrupt
	
	edge = 0;
	timer0_ovf = 0;
	state = RC_INIT;
}

uint8_t rc_get_code(rc_code_t *code)
{
	uint8_t ret = 0;

	ready = 0;
	while(!ready)
		;

	code->addr = rx_code[1]; // 8 bit address
	code->data = rx_code[0]; // 7 bit command

	return ret;
}

ISR(TIMER0_OVF_vect)
{
	if(~timer0_ovf) { /* increment only upto 0xFF */
		timer0_ovf++;
	}
}


ISR(INT0_vect)
{
	static uint8_t bit_count;  /* Number of bits to be received */
	static uint8_t data; /* received data in 8-bits each */
	static uint8_t pos; /* bit position */
	
	switch(state) {
		case RC_INIT:  
			if(!edge && (timer0_ovf >= IDLE_HIGH_COUNT)) { /* falling edge after >20ms high time? */
				state = RC_START;
			}
			timer0_ovf = 0;
			TCNT0 = 0; /* restart timer */
			break;
			
		case RC_START:
			if(edge && (TCNT0 > HEADER_LOW_COUNT)) { /* rising edge of header LOW pulse */
				TCNT0 = 0;
				state = RC_SAMP;
				bit_count = 0;
				data = 0;
				pos = 0;
			}
			else { /* short header LOW pulse */
				state = RC_INIT;
				timer0_ovf = 0;
			}
			break;
			
		case RC_SAMP:
			if(!edge) {  /* LOW edge: start timing LOW pulse */
				TCNT0 = 0;
			}
			else { /* HIGH edge: end of LOW pulse */
				if(TCNT0 > PULSE_THRESHOLD) {
					data |= (1 << pos);
				}
				pos++;
				bit_count++;
				if(bit_count == 7) {
					rx_code[0] = data; /* command */
					data = 0;
					pos = 0;
				}
				else if(bit_count == 15) {
					rx_code[1] = data; /* address */
					state = RC_INIT;
					timer0_ovf = 0;
					ready = 1;
				}
			}
			break;
			
		default : break; 
	}
	
	if(!edge) { /* This was falling edge, next rising edge */
		edge = 1;
		MCUCR |= (1 << 0);
	}
	else {	/* This was rising edge, next falling edge */
		edge = 0;
		MCUCR &= ~(1 << 0);
	}
}


