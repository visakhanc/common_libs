/* 
 *  Library to detect RC5 code pulses through TSOP1768 IR receiver
 *		- INT0 interrupt and Timer0 is used
 */
 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "ir_rc5.h"

#define RED_LED					PB0
#define RED_LED_DDR				DDRB
#define RED_LED_PORT			PORTB

#define RED_LED_OUT()			(RED_LED_DDR |= (1 << RED_LED))
#define RED_LED_ON()			(RED_LED_PORT |= (1 << RED_LED))
#define RED_LED_OFF()			(RED_LED_PORT &= ~(1 << RED_LED))
#define RED_LED_TOGGLE()		(RED_LED_PORT ^= (1 << RED_LED))

#if F_CPU == 1000000
	#define TCCR0_VAL 3		/* 64 us */
#elif F_CPU == 2000000
	#define TCCR0_VAL 3		/* 32 us */
#elif F_CPU == 4000000
	#define TCCR0_VAL 4 	/* 64 us */
#elif F_CPU == 8000000
	#define TCCR0_VAL 4		/* 32 us */
#elif F_CPU == 12000000		
	#define TCCR0_VAL 5		/* 85 us */
#else
	#error F_CPU not supported for ir_rc5.c, use another F_CPU
#endif

enum rc5_state {
	RC5_INIT = 0,
	RC5_START_1,
	RC5_START_2,
	RC5_SAMP
};

static rc5_code_t rx_code;
static uint8_t timer0_ovf;
static uint8_t edge;
static uint8_t time_T; /* Low + High time */
static enum rc5_state state; 
static volatile uint8_t ready = 0;

static void Timer0_Init(void)
{
	TIMSK |= (1 << 0);  // Enable Timer0 Overflow interrupt

	/* Start Timer0 */
	TCCR0 = TCCR0_VAL;
}

void rc5_init(void)
{
	Timer0_Init();
	
	/* External INT0 */
	MCUCR |= (1 << 1); // Falling edge generates interrupt
	MCUCR &= ~(1 << 0);
	GICR |= (1 << INT0); // Enable interrupt
	edge = 0;
	state = RC5_INIT;
}

uint8_t rc5_get_code(rc5_code_t *code)
{
	ready = 0;
	while(!ready)
		;
		
	code->data = rx_code.data;
	code->addr = rx_code.addr;
	return 0;
}

ISR(TIMER0_OVF_vect)
{
	timer0_ovf++;
}

int main(void)
{
	int8_t 		i;
	rc5_code_t 	code;
	
	RED_LED_OUT();
	RED_LED_ON();
	rc5_init();
	sei();
	
	while(1) {
		rc5_get_code(&code);
		if((code.data & 0x3F) == RC5_POWER) {
			RED_LED_TOGGLE();
		}
		
		for(i = 7; i >= 0; i--) {
			if(code.data & (1 << i)) {
				RED_LED_ON();
				_delay_ms(50);
				RED_LED_OFF();
				_delay_ms(50);
			}
			RED_LED_ON();
			_delay_ms(50);
			RED_LED_OFF();
			_delay_ms(50);
			
			_delay_ms(700);
		}
				
	}
}

ISR(INT0_vect)
{
	static uint8_t bit_count;
	static uint16_t data;
	
	switch(state) {
		case RC5_INIT:  
			if(!edge && (timer0_ovf > 4)) { /* falling edge after >30us high time? */
				state = RC5_START_1;
			}
			// else { /* some edge after last data bit? */
			timer0_ovf = 0;
			TCNT0 = 0; /* restart timer */
			break;
			
		case RC5_START_1:
			if(edge && (!timer0_ovf)) { /* rising edge and no overflow? */
				time_T = TCNT0;
				TCNT0 = 0;
				state = RC5_START_2;
			}
			else { /* should be overflow */
				state = RC5_INIT;
			}
			break;

		case RC5_START_2:
			if(!edge && !timer0_ovf) { /* falling edge and no overflow? */
				time_T += TCNT0;
				TCNT0 = 0;
				state = RC5_SAMP;
				data = 0;
				bit_count = 0;
			}
			else { /* should be overflow */
				state = RC5_INIT;
			}
			break;
		
		case RC5_SAMP:
			if((TCNT0 > (time_T - 3)) && (TCNT0 < (time_T + 3)) ) {  /* Sampling instant correct? */
				TCNT0 = 0;
				if(edge) { /* input bit and shift left */
					data |= 1;
				}
				data <<= 1;
				bit_count++;
				if(bit_count == 12) {
					rx_code.addr = (data >> 8);
					rx_code.data = (uint8_t)data;
					state = RC5_INIT;
					timer0_ovf = 0;
					ready = 1;
				}
			}
			else if(TCNT0 > 3*time_T) {
				timer0_ovf = 0;
				state = RC5_INIT;
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


