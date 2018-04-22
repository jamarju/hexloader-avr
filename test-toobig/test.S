#ifdef __AVR_ATmega328P__
    .set FLASH_SIZE, 32 * 1024
#elif __AVR_ATmega2560__
    .set FLASH_SIZE, 256 * 1024
#else
#error "Unsupported chip (see config.h)"
#endif

    .section .text
    .global main                ; Note [5]
main:
    jmp main

    .rept FLASH_SIZE / 19
    .byte 0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0
    .byte 0xef, 0xee, 0xed
    .endr

