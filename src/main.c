#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <main.h>

#define F_CPU 8000000UL

#define INPUT_PIN PB0

#define ADDRESS_LENGTH 4
#define COMMAND_LENGTH 3
#define PARAM1_LENGTH 3
#define PARAM2_LENGTH 3
#define PARAM3_LENGTH 3
#define FRAME_LENGTH 16

typedef union {
  struct {
    volatile uint8_t address : ADDRESS_LENGTH;  // 4 bits for address
    volatile uint8_t command : COMMAND_LENGTH;  // 3 bits for command
    volatile uint8_t param1 : PARAM1_LENGTH;   // 3 bits for param1
    volatile uint8_t param2 : PARAM2_LENGTH;   // 3 bits for param2
    volatile uint8_t param3 : PARAM3_LENGTH;   // 3 bits for param3
  } fields;
  volatile uint16_t rawPayload;
} CommandData;

volatile uint8_t ledState = 1;
volatile CommandData data;

void setup_clock() {
    // CCP = 0xD8;
	// CLKMSR = 0x00;
    // Set the clock prescaler to the maximum value (CLKDIV = 0)
    CCP = 0xD8; // Unlock the clock prescaler
    CLKPSR = 0; // Set the clock prescaler to 1 (fastest possible)
}

void setup_inout() {
    SET_PIN_OUT(PB1);           // set PB1 as output
    SET_PIN_IN(PB0);            // set PB0 as input
    SET_BIT(PUEB, PUEB0);   // pull-up PB0

    SET_BIT(PCICR, PCIE0);  // pin change interrupt control - enable interupt on pin change
    SET_BIT(PCMSK, PCINT0); // pin change mask - enable PCINT0 [1] as an interrupt source

    sei();
}

ISR(PCINT0_vect) {
    if(!(PINB & (1 << INPUT_PIN))) {  // if PB0 is low
        PIN_OFF(PB2);
        //_delay_us(50);
        for(uint8_t i = 0; i < FRAME_LENGTH; i++) {
            _delay_us(99);
            uint8_t pinValue = (PINB & (1 << INPUT_PIN));
            data.rawPayload = (data.rawPayload << 1) | pinValue;

            if (pinValue) {
                PIN_OFF(PB2);
                PIN_ON(PB2);
            } else {
                PIN_ON(PB2);
                PIN_OFF(PB2);
            }
        }      
        PIN_ON(PB2);
        
        _delay_ms(500);
        PIN_OFF(PB2);

        uint16_t value = data.rawPayload;
        uint16_t mask = 0x8000; // start with the most significant bit
        while(mask > 0) {
            // check if the current bit is 1 or 0
            if (value & mask) {
                do_sec_blink(PB1, 500);
            } else {
                do_sec_blink(PB1, 100);
            }
            mask >>= 1;
        }

        cli();

        PIN_ON(PB2);
        _delay_ms(200);
        PIN_OFF(PB2);
        _delay_ms(200);
        PIN_ON(PB2);
        _delay_ms(200);
        PIN_OFF(PB2);
        _delay_ms(200);
    }
    EIFR = 0x01; //clear interrupt
}

int do_handle_message() {
    _delay_ms(50);
    sei();  
    //do_blink(PB1, 300);
    //PIN_OFF(PB2); 
    return 0; 
}

void setup_blink() {
    SET_PIN_OUT(PB2);
}

inline static void do_blink(uint8_t pin, uint16_t delay_ms) {
    PIN_ON(pin);
    _delay_ms(delay_ms);
    PIN_OFF(pin);
    _delay_ms(delay_ms);
}

inline void do_sec_blink(uint8_t pin, uint16_t delay_ms) {
    PIN_ON(pin);
    _delay_ms(delay_ms);
    PIN_OFF(pin);
    _delay_ms(1000-delay_ms);
}

int main() {
    setup_clock();
    setup_inout();
    setup_blink();

    while (1) {
        PIN_ON(PB2);
        do_handle_message();
        //do_blink(PB2, 200);
    }

    return 0;
}
