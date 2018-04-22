ROOT=.
TARGET = test
SRC = $(wildcard *.c)
TEXT_SECTION = 0x0000
AVRDUDE_PROGRAMMER = arduino
AVRDUDE_PORT = /dev/tty.usbmodem411
UPLOAD_RATE = 115200

ifeq ($(ARCH), 328p)

MCU = atmega328p
F_CPU = 16000000L

else ifeq ($(ARCH), 2560)

MCU = atmega2560
F_CPU = 16000000L

else
$(error Use make ARCH=328p or ARCH=2560)
endif

all: typical lss

include ../Makefile.mk

