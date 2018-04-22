ROOT=.
TARGET = hexloader
SRC = $(wildcard *.c)
CXXSRC = $(wildcard *.cpp)
LIBS =

# Bootloader programmer
AVRDUDE_PROGRAMMER = avrisp2
AVRDUDE_PORT = /dev/tty.usbmodem00028961
UPLOAD_RATE = 115200

ifeq ($(ARCH), 328p)

MCU = atmega328p
F_CPU = 16000000L
TEXT_SECTION = 0x7000
LFUSE = 0xFF
HFUSE = 0xD0
EFUSE = 0x05

else ifeq ($(ARCH), 2560)

MCU = atmega2560
F_CPU = 16000000L
TEXT_SECTION = 0x3F000
LFUSE = 0xFF
HFUSE = 0xD2
EFUSE = 0x05

else
$(error Use make ARCH=328p or ARCH=2560)
endif

all: typical lss

include ../Makefile.mk
