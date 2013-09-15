// attinyX5 color cycler
// 8MHz system clock
// use a common-cathode RGB LED

// port assignments:
// PB0 (OC0A): red LED
// PB1 (OC0B): green LED
// PB2: unused
// PB3: push button
// PB4 (OC1B): blue LED

// placed in the public domain by the author
// Jeremy Stanley, September 2013

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

#include "patterns.h"

void init_io()
{
    DDRB  = (1 << DDB0) | (1 << DDB1) | (1 << DDB4);
    PORTB = (1 << PORTB0) | (1 << PORTB1) | (1 << PORTB3) | (1 << PORTB4);
}

unsigned char button_pressed()
{
    static unsigned char prev_button = (1 << PORTB3);
    unsigned char cur_button = PINB & (1 << PORTB3);
    unsigned char retval = cur_button && !prev_button;
    prev_button = cur_button;
    return retval;
}

ISR(TIMER1_OVF_vect)
{
    static unsigned char pattern = 0;
    static unsigned char rgb[3] = {0, 0, 0};
    
    if (button_pressed()) {
        if (++pattern == PATTERN_COUNT)
            pattern = 0;
    }

    (*PATTERNS[pattern])(rgb);
    
    // WTF? The answer is, TIMER0 runs in inverting PWM mode, because otherwise it can't provide a 0% duty cycle.
    // TIMER1, on the other hand, must run NON-inverting due to delayed latch of the output compare register...
    OCR0A = 255 - rgb[0];
    OCR0B = 255 - rgb[1];
    OCR1B = rgb[2];
}

void init_interrupts()
{
    // timer0: R/G
    TCCR0A = (1 << COM0A0) | (1 << COM0A1) | (1 << COM0B0) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00); // inverting(!) fast PWM mode
    TCCR0B = (1 << CS02); // 1/256 prescaler

    // timer1: B
    GTCCR = (1 << PWM1B) | (1 << COM1B1); // PWM mode, clear on compare match
    TCCR1 = (1 << CS13) | (1 << CS10);    // 1/256 prescaler
    
    // overflow interrupt
    TIMSK = (1 << TOIE1);
}

int main(void)
{
    init_io();
    init_interrupts();
    
    sei();
    
    // main loop is empty; everything is interrupt-driven
    for(;;){
        set_sleep_mode(SLEEP_MODE_IDLE);
        sleep_enable();
        sleep_cpu();
    }
    return 0;   // never reached
}
