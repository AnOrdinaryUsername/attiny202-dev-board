/* 
 * Tells delay.h that our clock is at 10 MHz.
 * This DOES NOT set the CPU clock speed.
 */
#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    /* Sets prescaler division factor to 2x and sets 
     * CPU clock speed to 10 MHz (20 MHz / 2).
     * 
     * Important because we power the device with 3.3V and don't want
     * to overclock the CPU (See Figure 31-1 pg. 477)!
     */
    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm);
    // Set Pin 1 in Port A as output
    // See 16.5.2
    PORTA.DIRSET = 0b00000010;
    
    while (1) {
        // Set output value of Pin 1 in Port A to High aka 1
        // See 16.5.6
        PORTA.OUTSET = 0b00000010;
        _delay_ms(500);
        // Clear the bit value of Pin 1 in Port A and set it to Low aka 0
        // See 16.5.7
        PORTA.OUTCLR = 0b00000010;
        _delay_ms(500);
    }
}