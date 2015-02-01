// attinyX5 pixel for RPi
// 8MHz system clock
// use a common-cathode RGB LED

// TODO (hardware):
// [ ] remove push button
// [ ] move red LED from PB0 (pin 5) to PB3 (pin 2)

// port assignments:
// PB0 (SDA): TWI input from RPi
// PB2 (SDA): TWI input from RPi
// PB1: green LED
// PB3: red LED
// PB4: blue LED

#define R_INTENSITY OCR0A
#define G_INTENSITY OCR0B
#define B_INTENSITY OCR1A

// placed in the public domain by the author
// Jeremy Stanley, September 2013

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

// software PWM for RGB
// (I don't have three PWM pins to spare after TWI)

void init_io()
{
    DDRB  = (1 << DDB3) | (1 << DDB1) | (1 << DDB4);
    PORTB = 0;
}

ISR(TIMER0_OVF_vect)
{
	PORTB &= ~((1 << PB3) | (1 << PB1));
}

ISR(TIMER0_COMPA_vect)
{
	PORTB |= (1 << PB3);
}

ISR(TIMER0_COMPB_vect)
{
	PORTB |= (1 << PB1);
}

ISR(TIMER1_OVF_vect)
{
	PORTB &= ~(1 << PB4);
}

ISR(TIMER1_COMPA_vect)
{
	PORTB |= (1 << PB4);
}

void init_interrupts()
{
    // timer0: R/G
    TCCR0A = 0;	// normal operation
    TCCR0B = (1 << CS02); // 1/256 prescaler

    // timer1: B
    GTCCR = 0; // normal operation
    TCCR1 = (1 << CS13) | (1 << CS12) | (1 << CS10); // 1/256 prescaler
        
    // interrupts: overflow on both timers; compare-match for each color
    TIMSK = (1 << TOIE0) | (1 << TOIE1) | (1 << OCIE0A) | (1 << OCIE0B) | (1 << OCIE1A);
    
    // values
    R_INTENSITY = 0;
    G_INTENSITY = 0;
	B_INTENSITY = 0;
}

void cycle(volatile uint8_t *pThing)
{
	uint8_t i;
	for(i = 0; i < 255; ++i) {
		*pThing = i;
		_delay_ms(10);
	}
	for(; i > 0; --i) {
		*pThing = i;
		_delay_ms(10);
	}	
	*pThing = 0;
	_delay_ms(500);
}

int main(void)
{
    init_io();
    init_interrupts();
    
    sei();
    
    for(;;){
    	cycle(&R_INTENSITY);
	   	cycle(&G_INTENSITY);
       	cycle(&B_INTENSITY);
        //set_sleep_mode(SLEEP_MODE_IDLE);
        //sleep_mode();
    }
    return 0;   // never reached
}
