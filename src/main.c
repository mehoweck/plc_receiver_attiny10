#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <main.h>
#include <blink.h>

#define F_CPU 8000000UL

#define ADDRESS 2

#define INPUT_PIN PB0
#define LED1_PIN PB1
#define LED2_PIN PB2
#define ADC_PIN PB3

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
volatile uint8_t address = 0;

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
    SET_PIN_IN(INPUT_PIN);        // set INPUT_PIN as input
    SET_PIN_OUT(LED1_PIN);       // set PB1 as output
    SET_PIN_OUT(LED2_PIN);       // set PB2 as output
    SET_BIT(PUEB, INPUT_PIN);   // pull-up INPUT_PIN

    SET_BIT(PCICR, INPUT_PIN);  // pin change interrupt control - enable interupt on pin change
    SET_BIT(PCMSK, INPUT_PIN); // pin change mask - enable PCINT0 [1] as an interrupt source

    SET_BIT(DIDR0, ADC_PIN);  // disable digital input buffer on analog input and digital outputs    
    sei();
}

void setup_pwm() {
    TCCR0A |= (1 << WGM01) | (1 << WGM00);  // Włączenie trybu Fast PWM (bit WGM01 i WGM00)
    TCCR0B |= (1 << CS01) | (1 << CS00);    // Ustawienie preskalera na 64 (bit CS01 i CS00)    
    
    // Ustawienie wartości rejestru OCR0A (Output Compare Register A)
    // Wartość odpowiadająca wypełnieniu 60%
    OCR0A = (uint8_t)(0.6 * 255);

    // Włączenie przerwań Compare Match A
    TIMSK0 |= (1 << OCIE0A);
}

uint8_t read_adc(uint8_t channel)
{
  ADMUX = channel;                      // set ADC channel
  ADCSRA = (1 << ADEN) |                // enable ADC
           (1 << ADSC) |                // start conversion
           (1 << ADPS1) | (1 << ADPS0); // set precsaler to 8
  while (ADCSRA & (1 << ADSC));         // wait until conversion is finished
  return ADCL;                          // Zwróć 8-bitową wartość z rejestru ADCH
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
        PIN_OFF(LED1_PIN);
        PIN_OFF(LED2_PIN);
        break;

    case CMD_LED_ON:
        PIN_ON(LED1_PIN);
        break;

    case CMD_LED_UP:
        PIN_ON(LED1_PIN);
        PIN_ON(LED2_PIN);
        break;

    case CMD_LED_DOWN:
        PIN_ON(LED2_PIN);
        break;

    case CMD_LED_AUTO:  //read ADC and calculate addres number depending on the value
    {
        //uint8_t adc_val = read_adc(ADC_PIN);

        blink(address, 3, LED2_PIN);
        break; 
    }
    
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

        uint8_t addrtmp = read_adc(ADC_PIN);
        if (address != addrtmp) {
            address = addrtmp;
            PIN_ON(LED1_PIN);
            _delay_ms(200);
            PIN_OFF(LED1_PIN);
        } 
        // uint8_t addrtmp = read_adc(ADC_PIN) >> 4;
        // if (address != addrtmp) {
        //     address = addrtmp;
        //     PIN_ON(LED1_PIN);
        //     _delay_ms(200);
        //     PIN_OFF(LED1_PIN);
        // } 
    }

    return 0;
}

