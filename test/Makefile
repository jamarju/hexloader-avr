ROOT=.
TARGET = test
SRC = $(wildcard *.c)
TEXT_SECTION = 0x0000
AVRDUDE_PROGRAMMER = arduino
AVRDUDE_PORT = /dev/tty.usbmodem411
UPLOAD_RATE = 115200

all: typical lss

include ../Makefile.mk

