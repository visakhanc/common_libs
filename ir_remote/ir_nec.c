/* 
 *  Library to detect remote control code pulses through TSOP18xx IR receiver
 *		
 */
 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "ir_nec.h"

#if F_CPU == 1000000
	#define TCCR0_VAL 			3	 /* 64 us */
	#define HEADER_LOW_COUNT	125  /* For 64us timer, 8ms = 125 counts*/
	#define HEADER_HIGH_COUNT	62	 /* 4 ms */
	#define HEADER_HIGH2_COUNT	31	 /* 2 ms for repeat code */
	#define PULSE_THRESHOLD		16   /* 1ms */
	#define IDLE_HIGH_COUNT		2	 /* 30 ms */
#elif F_CPU == 2000000
	#define TCCR0_VAL 			4	 /* 128 us */
	#define HEADER_LOW_COUNT	62  /* For 128us timer, 8ms = 62 counts */
	#define HEADER_HIGH_COUNT	31	 /* 4 ms */
	#define HEADER_HIGH2_COUNT	16	 /* 2 ms for repeat code*/
	#define PULSE_THRESHOLD		8   /* 1ms */
	#define IDLE_HIGH_COUNT		1	 /* 30 ms */
#elif F_CPU == 4000000
	#define TCCR0_VAL 			4 	/* 64 us */
	#define HEADER_LOW_COUNT	125  /* For 64us timer, 8ms = 125 counts*/
	#define HEADER_HIGH_COUNT	62	 /* 4 ms */
	#define HEADER_HIGH2_COUNT	31	 /* 2 ms for repeat code */
	#define PULSE_THRESHOLD		16   /* 1ms */
	#define IDLE_HIGH_COUNT		2	 /* 30 ms */
#elif F_CPU == 8000000
	#define TCCR0_VAL 			5	 /* 128 us */
	#define HEADER_LOW_COUNT	62  /* For 128us timer, 8ms = 62 counts */
	#define HEADER_HIGH_COUNT	31	 /* 4 ms */
	#define HEADER_HIGH2_COUNT	16	 /* 2 ms for repeat code */
	#define PULSE_THRESHOLD		8   /* 1ms */
	#define IDLE_HIGH_COUNT		1	 /* 30 ms */
#elif F_CPU == 12000000
	#define TCCR0_VAL 			5	 /* 85 us */
	#define HEADER_LOW_COUNT	94  /* For 85us timer, 8ms = 94 counts */
	#define HEADER_HIGH_COUNT	47	 /* 4 ms */
	#define HEADER_HIGH2_COUNT	23	 /* 2 ms for repeat code */
	#define PULSE_THRESHOLD		12   /* 1ms */
	#define IDLE_HIGH_COUNT		1	 /* 30 ms */
#else
	#error F_CPU not supported for ir_nec.c, use another F_CPU
#endif


#if !((IR_INTERRUPT == INT0) || (IR_INTERRUPT == INT1))
	#error Interrupt pin for IR receiver not defined or invalid definition; Define properly in ir_config.h
#endif 

enum rc_state {
	RC_INIT = 0,
	RC_START_1,
	RC_START_2,
	RC_SAMP
};

static uint8_t rx_code[4];
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
	
#if IR_INTERRUPT == INT0 
	/* External INT0 */
	MCUCR |= (1 << ISC01); // Falling edge generates interrupt
	MCUCR &= ~(1 << ISC00);
	GICR |= (1 << INT0); // Enable interrupt
#else 
	/* External INT1 */
	MCUCR |= (1 << ISC11); // Falling edge generates interrupt
	MCUCR &= ~(1 << ISC10);
	GICR |= (1 << INT1); // Enable INT1 interrupt
#endif

	edge = 0;
	timer0_ovf = 0;
	state = RC_INIT;
}

/* Waits for code to be received and returns it 
	Return: 0 - success
			1 - error in received code */
uint8_t rc_wait_get(rc_code_t *code)
{
	uint8_t ret = 0;

	ready = 0;
	while(!ready)
		;
		
	/* Check received codes are OK */
	if((rx_code[0] & rx_code[1]) || (rx_code[2] & rx_code[3])) {
		ret = 1;
	}

	code->addr = rx_code[0];
	code->data = rx_code[2];

	return ret;
}


/* Provides received code if any
	Return:
		No code is received or if error in code - 0
		Code received OK - 1 
*/
uint8_t rc_get(rc_code_t *code)
{
	uint8_t ret = 0;
	
	if(ready) {
		ready = 0;
		ret = 1;
		/* Check received codes are OK */
		if((rx_code[0] & rx_code[1]) || (rx_code[2] & rx_code[3])) {
			ret = 0; // error with received code
		}
		code->addr = rx_code[0];
		code->data = rx_code[2];
	}
	
	return ret;
}


ISR(TIMER0_OVF_vect)
{
	if(~timer0_ovf) { /* increment only upto 0xFF */
		timer0_ovf++;
	}
}

#if IR_INTERRUPT == INT0
ISR(INT0_vect)
#else
ISR(INT1_vect)
#endif
{
	static uint8_t bit_count;  /* present bits in data */
	static uint8_t data; /* received data in 8-bits each */
	static uint8_t byte_count; /* bytes of data received */
	
	switch(state) {
		case RC_INIT:  
			if(!edge && (timer0_ovf >= IDLE_HIGH_COUNT)) { /* falling edge after >30ms high time? */
				state = RC_START_1;
			}
			timer0_ovf = 0;
			TCNT0 = 0; /* restart timer */
			break;
			
		case RC_START_1:
			if(edge && (TCNT0 > HEADER_LOW_COUNT)) { /* rising edge of header LOW pulse */
				TCNT0 = 0;
				state = RC_START_2;
			}
			else { /* short header LOW pulse */
				state = RC_INIT;
				timer0_ovf = 0;
			}
			break;
			
		case RC_START_2:
			if(!edge) { 
				if(TCNT0 > HEADER_HIGH_COUNT) { /* falling edge of header HIGH pulse */
					TCNT0 = 0;
					state = RC_SAMP;
					byte_count = 0;
				}
				else if(TCNT0 > HEADER_HIGH2_COUNT) { /* falling edge of header in repeat code */
					TCNT0 = 0;
					timer0_ovf = 0;
					state= RC_INIT;
					ready = 1;  /* Take the previously received code */
				}
				else { /* short header HIGH pulse*/
					state = RC_INIT;
					timer0_ovf = 0;
				}
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
#if IR_INTERRUPT == INT0
		MCUCR |= (1 << ISC00);
#else
		MCUCR |= (1 << ISC10);
#endif
	}
	else {	/* This was rising edge, next falling edge */
		edge = 0;
#if IR_INTERRUPT == INT0
		MCUCR &= ~(1 << ISC00);
#else
		MCUCR &= ~(1 << ISC10);
#endif
	}
}


