#ifndef MAIN_H_
#define MAIN_H_

#include <avr/io.h>

#define SET_BIT(REG, BIT) REG |= 1 << BIT
#define CLR_BIT(REG, BIT) REG &= ~(1 << BIT)

#define SET_PIN_IN(PIN) CLR_BIT(DDRB, PIN)
#define SET_PIN_OUT(PIN) SET_BIT(DDRB, PIN)

#define PIN_ON(PIN) PORTB |= 1 << PIN
#define PIN_OFF(PIN) PORTB &= ~(1 << PIN)

#define IS_LO(PIN) !(PINB & (1 << PIN))
#define IS_HI(PIN) (PINB & (1 << PIN))

enum CommandCode {
  CMD_LED_OFF  = 1, // turn LED off
  CMD_LED_ON   = 2, // turn LED on (param1 - light level 0-7) 
  CMD_LED_UP   = 3, // smoothly raise the LED brightness to the level given by param1 for time in param2 
  CMD_LED_DOWN = 4, // smoothly dimm to the level given by param1
  CMD_LED_AUTO = 5, // set the LED light level automaticaly
  CMD_LED_RGB  = 6, // turn the RBG color given by params[1-3] 
  CMD_FUTURE2  = 7, // future use
  CMD_FUTURE3  = 0  // future use
}; 

#endif /*MAIN_H_*/