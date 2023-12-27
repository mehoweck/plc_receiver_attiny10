#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <main.h>

#define F_CPU 8000000UL

#define ADDRESS_LENGTH 4
#define COMMAND_LENGTH 3
#define PARAM1_LENGTH 3
#define PARAM2_LENGTH 3
#define PARAM3_LENGTH 3
//#define FRAME_LENGTH 16
typedef union {
  struct {
    unsigned int  address : ADDRESS_LENGTH;  // 4 bits for address
    unsigned int  command : COMMAND_LENGTH;  // 3 bits for command
    unsigned int  param1 : PARAM1_LENGTH;   // 3 bits for param1
    unsigned int  param2 : PARAM2_LENGTH;   // 3 bits for param2
    unsigned int  param3 : PARAM3_LENGTH;   // 3 bits for param3
  } fields;
  uint8_t rawBytes[2];
} CommandData;

volatile uint8_t ledState = 1;

void setup_clock() {
    // CCP = 0xD8;
	// CLKMSR = 0x00;
    // Set the clock prescaler to the maximum value (CLKDIV = 0)
    CCP = 0xD8; // Unlock the clock prescaler
    CLKPSR = 0; // Set the clock prescaler to 1 (fastest possible)
}

void setup_inout() {
    PIN_OUT(PB1);           // set PB1 as output
    PIN_IN(PB0);            // set PB0 as input
    SET_BIT(PUEB, PUEB0);   // pull-up PB0

    SET_BIT(PCICR, PCIE0);  // pin change interrupt control - enable interupt on pin change
    SET_BIT(PCMSK, PCINT0); // pin change mask - enable PCINT0 [1] as an interrupt source

    sei();
}

ISR(PCINT0_vect) {
    PIN_ON(PB2);
    cli();
    if(!(PINB & (1 << PB0))) {  // if PB0 is low
        ledState = !ledState;

        // Write new LED state to PB1
        if(ledState) {
            PIN_ON(PB1); // Turn on LED
        } else {
            PORTB &= ~(1 << PB1); // Turn off LED
        }
    }
}

int do_handle_message() {
    _delay_ms(150);
    sei();  
    PIN_OFF(PB2); 
    return 0; 
}

void setup_blink() {
    PIN_OUT(PB2);
}

void do_blink(uint8_t pin, uint8_t delay_ms) {
    PIN_ON(pin);
    _delay_ms(500);
    PIN_OFF(pin);
    _delay_ms(500);
}

int main() {
    setup_clock();
    setup_inout();

    setup_blink();
    while (1) {
        do_handle_message();
        //do_blink(PB2, 200);
    }

    return 0;
}
