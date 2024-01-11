#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <main.h>
#include <blink.h>

#define F_CPU 8000000UL

#define ADDRESS 2

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
    volatile uint16_t rawData;
} Command;

volatile Command command;

volatile uint8_t readMode = 0;
volatile uint8_t readBitsCount = 0;
volatile uint8_t commandReady = 0;

void setup_clock() {
    // CCP = 0xD8;
	// CLKMSR = 0x00;
    // Set the clock prescaler to the maximum value (CLKDIV = 0)
    CCP = 0xD8; // Unlock the clock prescaler
    CLKPSR = 0; // Set the clock prescaler to 1 (fastest possible)
}

void setup_inout() {
    SET_PIN_IN(PB0);            // set PB0 as input
    SET_PIN_OUT(PB1);           // set PB1 as output
    SET_PIN_OUT(PB2);           // set PB2 as output
    SET_BIT(PUEB, PUEB0);   // pull-up PB0

    SET_BIT(PCICR, PCIE0);  // pin change interrupt control - enable interupt on pin change
    SET_BIT(PCMSK, PCINT0); // pin change mask - enable PCINT0 [1] as an interrupt source

    PIN_ON(PB2);    
    sei();
}

ISR(PCINT0_vect) {     
    uint8_t raisingEdge = IS_HI(INPUT_PIN);
    if (raisingEdge) {
        if (readBitsCount++ < FRAME_LENGTH) {
            _delay_us(50);  // wait for the middle of the transmitted bit
            command.rawData = (command.rawData << 1) | IS_HI(INPUT_PIN);
        } else {
            // HIGH state set after the command is sent
            readMode = 0;
            commandReady = IS_HI(INPUT_PIN) & ((ADDRESS & command.fields.address) > 0);
        }
    } else { // if falling edge
        if (!readMode) { // new command
            readMode = 1;
            readBitsCount = 0;
            commandReady = 0;
        }
    }
}

void handle_command(Command command) {
    switch (command.fields.command) {
    
    case CMD_LED_OFF:
        PIN_OFF(PB1);
        PIN_OFF(PB2);
        break;

    case CMD_LED_ON:
        PIN_ON(PB1);
        break;

    case CMD_LED_UP:
        PIN_ON(PB1);
        PIN_ON(PB2);
        break;

    case CMD_LED_DOWN:
        PIN_ON(PB2);
        break;
    
    default:
        break;
    }
}

int main() {
    setup_clock();
    setup_inout();

    while (1) {
        if (commandReady) {
            cli();
            handle_command(command);
            commandReady = 0;
            sei();
        }
    }

    return 0;
}
