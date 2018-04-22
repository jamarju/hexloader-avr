#ifdef __AVR_ATmega328P__
    .set NRWW_START, 0x7000
#elif __AVR_ATmega2560__
    .set NRWW_START, 0x3E000
#else
#error "Unsupported chip (see config.h)"
#endif

    .set RUNTIME_SIZE, 0x100

    .section .text
    .global main                ; Note [5]
main:
    jmp main
   
    .rept (NRWW_START - RUNTIME_SIZE) / 19
    .byte 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    .byte 0x10, 0x11, 0x12
    .endr

