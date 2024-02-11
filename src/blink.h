#ifndef BLINK_H
#define BLINK_H

#include <avr/io.h>

void blink(uint16_t data, uint8_t bitsCount, uint8_t pin);
void blink8(uint8_t data, uint8_t pin);
void blink16(uint16_t data, uint8_t pin);

#endif // BLINK_H