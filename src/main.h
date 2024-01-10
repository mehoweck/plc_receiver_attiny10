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

//#define PIN_ON(PORT, PIN) PORT |= 1 << PIN
//#define PIN_OFF(PORT, PIN) PORT &= ~(1 << PIN)



#endif /*MAIN_H_*/