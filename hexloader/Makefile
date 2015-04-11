ROOT=.
TARGET = hexloader
SRC = $(wildcard *.c)
CXXSRC = $(wildcard *.cpp)
LIBS =

# Bootloader programmer
AVRDUDE_PROGRAMMER = avrisp2
AVRDUDE_PORT = /dev/tty.usbmodem00028961
UPLOAD_RATE = 115200

TEXT_SECTION = 0x7000
LFUSE = 0xFF
HFUSE = 0xD0
EFUSE = 0x05

all: typical lss

include ../Makefile.mk
