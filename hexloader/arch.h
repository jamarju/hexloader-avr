#pragma once

#include <avr/pgmspace.h>
#include <stdint.h>

// Functions

void power_init(void);

// Architecture-dependent defines, macros and typedefs

#ifdef __AVR_ATmega328P__

#define FLASH_SIZE                  0x8000          ///< atmega328p total flash
#define NRWW_START                  0x7000          ///< can't flash beyond this while uart code runs
#define PAGE_SIZE                   0x80            ///< atmega328p page size

#define INIT_LED() DDRB |= _BV(DDB5);
#define LED_ON() PORTB |= _BV(PORTB5)       /**< Turn on the LED */
#define LED_OFF() PORTB &= ~_BV(PORTB5)     /**< Turn off the LED */

typedef uint16_t addr_t;

//#define S(x) ((addr_t)(const PROGMEM char *)(x)) 
//#define R(x) pgm_read_byte_near(x)
//#define RW(x) pgm_read_word_near(x)


#elif __AVR_ATmega2560__

#define FLASH_SIZE                  0x40000         ///< atmega2560 total flash
#define NRWW_START                  0x3e000         ///< can't flash beyond this while uart code runs
#define PAGE_SIZE                   0x100           ///< atmega2560 page size

#define USART_RX_vect               USART0_RX_vect
#define USART_UDRE_vect             USART0_UDRE_vect

#define INIT_LED() DDRB |= _BV(DDB7);
#define LED_ON() PORTB |= _BV(PORTB7)       /**< Turn on the LED */
#define LED_OFF() PORTB &= ~_BV(PORTB7)     /**< Turn off the LED */

typedef uint32_t addr_t;

#define P(x) (x)
//#define P(x) (uint32_t)pgm_get_far_address(x)
#define R(x) pgm_read_byte_far(x)
//#define RW(x) pgm_read_word_far(x)

#else
#error "Unsupported chip (see config.h)"
#endif

