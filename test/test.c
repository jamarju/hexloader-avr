#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "uart.h"


void reset() 
{
    asm volatile ("mov r2, 0");
    wdt_enable(WDTO_15MS);
    for (;;);
}

int main()
{
    // init uart and timer
    uart_init();
    sei();

    uart_send_string(PSTR("Hello world, press R to reboot\r\n"));

    while (1)
        if (uart_available())
            if (uart_recv_byte() == 'R')
                reset();
}

