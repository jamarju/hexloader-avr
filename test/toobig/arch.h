#pragma once

#include <avr/pgmspace.h>
#include <stdint.h>

// Functions

void power_init(void);

// Architecture-dependent defines, macros and typedefs

#ifdef __AVR_ATmega328P__


#elif __AVR_ATmega2560__

#define USART_RX_vect               USART0_RX_vect
#define USART_UDRE_vect             USART0_UDRE_vect


#else
#error "Unsupported chip (see config.h)"
#endif

