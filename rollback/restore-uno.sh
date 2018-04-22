#!/bin/bash

# This puts the original optiboot bootloader back in the Arduino.
# This is the stock bootloader that comes with:
#
# - Arduino Uno
#

# === BEGIN CUSTOMIZE THIS ===
ARDUINO="/Applications/Arduino.app/Contents/Java/hardware"
PROGRAMMER=avrisp2
SPEED=115200
PORT="/dev/tty.usbmodem00028961"
# === END CUSTOMIZE THIS ===

# These can be mostly left alone:
AVRDIR="$ARDUINO/tools/avr"
DUDE="$AVRDIR/bin/avrdude"
DUDECONF="$AVRDIR/etc/avrdude.conf"
BOOTLOADER="$ARDUINO/arduino/avr/bootloaders/optiboot/optiboot_atmega328.hex"
LFUSE=0xFF
HFUSE=0xDE
EFUSE=0xFD
CHIP=atmega328p

"$DUDE" -c $PROGRAMMER -C "$DUDECONF" -p $CHIP -P $PORT -b $SPEED -e \
  -U lock:w:0xFF:m \
  -U "flash:w:$BOOTLOADER:i" \
  -U lfuse:w:$LFUSE:m -U hfuse:w:$HFUSE:m -U efuse:w:$EFUSE:m \
  -U lock:w:0xCF:m
