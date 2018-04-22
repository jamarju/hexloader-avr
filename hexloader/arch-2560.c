#include <avr/io.h>
#include "arch.h"

#ifdef __AVR_ATmega2560__

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

    // Disable TWI, all timers, USARTS except USART0, SPI and ADC.
    // Note: SPI is need if debugging.
    PRR0 = _BV(PRTWI) | _BV(PRTIM2) | _BV(PRTIM1) | _BV(PRSPI) | _BV(PRADC);
    PRR1 = _BV(PRTIM5) | _BV(PRTIM4) | _BV(PRTIM3) | _BV(PRUSART3) | _BV(PRUSART2) 
        | _BV(PRUSART1);
}

#endif