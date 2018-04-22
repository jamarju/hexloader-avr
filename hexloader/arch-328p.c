#include <avr/io.h>
#include "arch.h"

#ifdef __AVR_ATmega328P__

///////////////////////////////////////////////////////////////////////
// Powersave utils
///////////////////////////////////////////////////////////////////////

/**
 * Sets the AVR to maximum power saving.
 * It does so by disabling unneeded modules. Also prepares the SMCR
 * to enter idle mode when the sleep instruction is executed.
 */
void power_init(void)
{
    // Idle mode is the only mode that will keep the UART running
    SMCR = _BV(SE);     // enable sleep instruction, idle mode

    // Disable TWI, Timer 2, Timer 1, SPI and ADC.
    // Note: SPI is need if debugging.
    PRR = _BV(PRTWI) | _BV(PRTIM2) | _BV(PRTIM1) | _BV(PRSPI) | _BV(PRADC);
}

#endif