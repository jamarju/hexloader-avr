#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "uart.h"

void __attribute__((noreturn)) reboot(uint8_t to_bootloader) 
{
    if (to_bootloader) {
        asm volatile ("eor r2, r2");
    } else {
        asm volatile (
            "ldi r24, 0xb0\n"
            "mov r2, r24\n"
            "ldi r24, 0xaa\n"
            "mov r3, r24\n" ::: "r24");
    }
    wdt_enable(WDTO_15MS);

    for (;;);
    __builtin_unreachable();    // suppress 'noreturn does return' warning
}


int main(void)
{
    // init uart and timer
    uart_init();
    sei();

    uart_send_string(PSTR("Hello world, press 'r' to reboot, 'b' for bootloader\r\n"));

    while (1)
        if (uart_available())
            switch(uart_recv_byte()) {
                case 'r': 
                    uart_send_string(PSTR("Rebooting to app...\r\n"));
                    reboot(0);
                case 'b': 
                    uart_send_string(PSTR("Rebooting to bootloader...\r\n"));
                    reboot(1);
            }
}

