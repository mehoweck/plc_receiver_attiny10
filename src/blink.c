#include <avr/io.h>
#include <util/delay.h>

void do_blink_hi(uint8_t pin) {
    PORTB |= 1 << pin;
    _delay_ms(100);
    PORTB &= ~(1 << pin);
    _delay_ms(50);
}

void do_blink_lo(uint8_t pin) {
    PORTB |= 1 << pin;
    _delay_ms(25);
    PORTB &= ~(1 << pin);
    _delay_ms(125);
}

void blink(uint16_t data, uint8_t bitsCount) {
    uint16_t mask = 1 << bitsCount;
    while(mask > 0) {
        if (data & mask) {
            do_blink_hi(PB1);
        } else {
            do_blink_lo(PB1);
        }
        mask >>= 1;
    }
}
void blink8(uint8_t data) { blink(data, 7); }
void blink16(uint16_t data) { blink(data, 15); }