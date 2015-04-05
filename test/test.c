#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "uart.h"

uint8_t mcusr __attribute__ ((section (".noinit")));

void read_mcusr() __attribute__ ((naked)) __attribute__ ((section (".init0")));
void read_mcusr()
{
  //register uint8_t r2 asm("r2");
  //mcusr = r2;
  asm volatile ("sts mcusr, r2");
}

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

    uart_send_string(PSTR("MCUSR="));
    uart_send_int(mcusr);

    while (1)
        if (uart_available())
            if (uart_recv_byte() == 'R')
                reset();
    //digitalWrite(13, HIGH);
    //delay(500);
    //digitalWrite(13, LOW);
    //delay(500);
}

