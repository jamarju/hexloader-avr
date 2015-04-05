#ifndef __UART_H__
#define __UART_H__

void uart_init();
void uart_send_byte(uint8_t c);
void uart_flush();
void uart_send_string(char const s[]);
void uart_send_int(uint16_t x);
void uart_send_byte_hex(uint8_t x);
int16_t uart_recv_byte();
int8_t uart_available();


#endif

