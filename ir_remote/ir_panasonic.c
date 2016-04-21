/* 
 *  Library to detect remote control code pulses through TSOP18xx IR receiver
 *		- INT0 interrupt and Timer0 is used
 */
 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "ir_panasonic.h"

#if F_CPU == 1000000
	#define TCCR0_VAL 			3	 /* 64 us */
	#define HEADER_LOW_COUNT	50  /* For 64us timer, 3.2ms = 50 counts */
	#define HEADER_HIGH_COUNT	25	 /* 1.6 ms */
	#define PULSE_THRESHOLD		16   /* 1ms */
#elif F_CPU == 2000000
	#define TCCR0_VAL 			3	 /* 32 us */
	#define HEADER_LOW_COUNT	100  /* For 32us timer, 3.2ms = 100 counts */
	#define HEADER_HIGH_COUNT	50	 /* 1.6 ms */
	#define PULSE_THRESHOLD		32   /* 1ms */
#elif F_CPU == 4000000
	#define TCCR0_VAL 			4 	/* 64 us */
	#define HEADER_LOW_COUNT	50  /* For 64us timer, 3.2ms = 50 counts */
	#define HEADER_HIGH_COUNT	25	 /* 1.6 ms */
	#define PULSE_THRESHOLD		16   /* 1ms */
#elif F_CPU == 8000000
	#define TCCR0_VAL 			4	 /* 32 us */
	#define HEADER_LOW_COUNT	100  /* For 32us timer, 3.2ms = 100 counts */
	#define HEADER_HIGH_COUNT	50	 /* 1.6 ms */
	#define PULSE_THRESHOLD		32   /* 1ms */
#elif F_CPU == 12000000
	#define TCCR0_VAL 			5	 /* 85 us */
	#define HEADER_LOW_COUNT	38  /* For 85us timer, 3.2ms = 38 counts */
	#define HEADER_HIGH_COUNT	19	 /* 1.6 ms */
	#define PULSE_THRESHOLD		12   /* 1ms */
#else
	#error F_CPU not supported for ir_panasonic.c, use another F_CPU
#endif



enum rc_state {
	RC_INIT = 0,
	RC_START_1,
	RC_START_2,
	RC_SAMP
};

static uint8_t rx_code[6];
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
	ready = 0;
	while(!ready)
		;
		
	code->addr[0] = rx_code[0];
	code->addr[1] = rx_code[1];
	code->data[0] = rx_code[2];
	code->data[1] = rx_code[3];
	code->data[2] = rx_code[4];
	code->data[3] = rx_code[5];

	return 0;
}

ISR(TIMER0_OVF_vect)
{
	timer0_ovf++;
}


ISR(INT0_vect)
{
	static uint8_t bit_count;  /* present bits in data */
	static uint8_t data; /* received data in 8-bits each */
	static uint8_t byte_count; /* bytes of data received */
	
	switch(state) {
		case RC_INIT:  
			if(!edge && (timer0_ovf > 4)) { /* falling edge after >30ms high time? */
				state = RC_START_1;
			}
			timer0_ovf = 0;
			TCNT0 = 0; /* restart timer */
			break;
			
		case RC_START_1:
			if(edge && (TCNT0 > HEADER_LOW_COUNT)) { /* rising edge after header LOW pulse */
				TCNT0 = 0;
				state = RC_START_2;
			}
			else { /* short header LOW pulse */
				state = RC_INIT;
				timer0_ovf = 0;
			}
			break;
			
		case RC_START_2:
			if(!edge && (TCNT0 > HEADER_HIGH_COUNT)) { /* falling edge after header HIGH pulse */
				TCNT0 = 0;
				state = RC_SAMP;
				byte_count = 0;
			}
			else { /* short header HIGH pulse*/
				state = RC_INIT;
				timer0_ovf = 0;
			}
			break;
		
		case RC_SAMP:
			if(edge) {  /* start timing HIGH pulse */
				TCNT0 = 0;
			}
			else { /* end of HIGH pulse */
				if(TCNT0 > PULSE_THRESHOLD) { 
					data |= 1;
				}
				bit_count++;
				if(bit_count == 8) {
					rx_code[byte_count++] = data;
					data = 0;
					bit_count = 0;
					if(byte_count == sizeof(rx_code)) {
						state = RC_INIT;
						timer0_ovf = 0;
						ready = 1;
					}
				}
				data <<= 1;
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


