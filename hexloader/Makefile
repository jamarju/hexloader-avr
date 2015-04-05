ROOT=.
TARGET = hexloader
SRC = $(wildcard *.c)
CXXSRC = $(wildcard *.cpp)
LIBS =
TEXT_SECTION = 0x7000
LFUSE = 0xFF
HFUSE = 0xD0
EFUSE = 0x05

all: typical lss

include ../Makefile.mk

