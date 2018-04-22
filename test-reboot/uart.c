#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "arch.h"

#define UBRR                        16      //< 34 = 56.6K, 16 = 115.2K bps, 8 = 230.4K bps
#define RX_BUFFER_LEN               1024    //< receive buffer length
#define TX_BUFFER_LEN               32      //< transmit buffer length

#define ERROR_RX_DATA_OVERRUN       1       ///< UART data overrun
#define ERROR_RX_FRAME_ERROR        2       ///< UART frame error
#define ERROR_RX_BUFFER_OVERFLOW    4       ///< UART #rx_buffer overflow

// Macros

#define LED_ON() PORTB |= _BV(PORTB5)       /**< Turn on the LED */
#define LED_OFF() PORTB &= ~_BV(PORTB5)     /**< Turn off the LED */

/** Sleep while condition holds true.  */
#define IDLE_WHILE(condition) \
    do { \
        cli(); \
        if (condition) { \
            sleep_enable(); \
            sei(); \
            sleep_cpu(); \
            sleep_disable(); \
        } else { \
            sei(); \
            break; \
        } \
        sei(); \
    } while (1);


// Variables

volatile uint8_t rx_buffer[RX_BUFFER_LEN];  ///< UART receive buffer
volatile uint8_t tx_buffer[TX_BUFFER_LEN];  ///< UART transmit buffer
volatile uint16_t rx_head, rx_tail, tx_head, tx_tail;
volatile uint8_t uart_error;                ///< one of #ERROR_RX_DATA_OVERRUN, #ERROR_RX_FRAME_ERROR or #ERROR_RX_BUFFER_OVERFLOW   

///////////////////////////////////////////////////////////////////////
// ISR routines 
///////////////////////////////////////////////////////////////////////

/**
 * UART RX ISR.
 * Called when the hardware UART receives a byte.
 */
ISR(USART_RX_vect)
{
    uint8_t status = UCSR0A;
    uint8_t data = UDR0;
    uint16_t new_head = (rx_head + 1) % RX_BUFFER_LEN;

    if (status & _BV(DOR0))
        uart_error |= ERROR_RX_DATA_OVERRUN;
    if (status & _BV(FE0))
        uart_error |= ERROR_RX_FRAME_ERROR;

    // If head meets tail -> overflow and ignore the received byte
    if (new_head == rx_tail) {
        uart_error |= ERROR_RX_BUFFER_OVERFLOW;
    }
    else {
        rx_buffer[rx_head] = data;
        rx_head = new_head;
    }
    // delay watchdog reboot while pending rx data
    wdt_reset();
}

/**
 * UART Data Register Empty ISR.
 * Called when the UART is ready to accept a new byte for transmission.
 */
ISR(USART_UDRE_vect)
{
    if (tx_head == tx_tail) {
        // Buffer is empty, disable UDRE int
        UCSR0B &= ~_BV(UDRIE0);
    }
    else {
        // Send byte
        UDR0 = tx_buffer[tx_tail];
        tx_tail = (tx_tail + 1) % TX_BUFFER_LEN;
    }
    // delay the reboot until no more pending tx
    wdt_reset();
}


///////////////////////////////////////////////////////////////////////
// UART functions
///////////////////////////////////////////////////////////////////////

/**
 * Start the UART.
 * Set the UART to 2x speed mode, 115.2K bauds, 8N1 and enable rx 
 * interrupts.
 */
void uart_init(void)
{
    // Set baud rate
    UBRR0H = (uint8_t) (UBRR >> 8);
    UBRR0L = (uint8_t) (UBRR);

    // double speed
    UCSR0A = _BV(U2X0);

    // Enable receiver and transmitter, generate interrupts on RX, DRE
    UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);

    // 8,N,1
    UCSR0C = (3 << UCSZ00);
}

/**
 * Send a byte.
 * If the tx queue is full, it will sleep (ie. block) until space becomes
 * available.
 * @param c the byte
 */
void uart_send_byte(uint8_t c)
{
    uint16_t new_head = (tx_head + 1) % TX_BUFFER_LEN;

    IDLE_WHILE(tx_tail == new_head);

    tx_buffer[tx_head] = c;
    cli();      // atomically update the head
    tx_head = new_head;
    sei();

    // Enable UDRE int, this will trigger the UDRE ISR
    UCSR0B |= _BV(UDRIE0);
}

/**
  * Flush the tx buffer.
  */
void uart_flush(void)
{
    IDLE_WHILE(tx_tail != tx_head);
}

/**
 * Send a PROGMEM string.
 * @param s the string, which *must* be in program space
 */
void uart_send_string(char const s[])
{
    char c;
    while ((c = pgm_read_byte(s++)))
        uart_send_byte(c);
}

/**
 * Send a uint16_t in decimal format.
 * @param x the uint16_t value
 */
void uart_send_int(uint16_t x)
{
    if (x < 10)
        uart_send_byte(x + '0');
    else {
        uart_send_int(x / 10);
        uart_send_byte(x % 10 + '0');
    }
}

/**
 * Receive a byte.
 * @return an int16_t with the byte, or -1 if no data available
 */
int16_t uart_recv_byte(void) 
{
    if (rx_tail == rx_head)
        return -1;    // no data

    cli();
    int16_t c = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUFFER_LEN;
    sei();

    return c;
}

/**
 * Check if there is incoming data over the UART.
 * @return true if data available
 */
int8_t uart_available(void)
{
    return (rx_tail != rx_head);
}

