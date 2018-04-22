#!/bin/bash

# This puts the original stk500v2 bootloader back in the Arduino.
# This is the stock bootloader that comes with:
#
# - Arduino MEGA 2560
# - RobotDyn MEGA 2560
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
BOOTLOADER="$ARDUINO/arduino/avr/bootloaders/stk500v2/stk500boot_v2_mega2560.hex"
LFUSE=0xFF
HFUSE=0xD8
EFUSE=0xFD
CHIP=atmega2560

"$DUDE" -c $PROGRAMMER -C "$DUDECONF" -p $CHIP -P $PORT -b $SPEED -e \
  -U lock:w:0xFF:m \
  -U "flash:w:$BOOTLOADER:i" \
  -U lfuse:w:$LFUSE:m -U hfuse:w:$HFUSE:m -U efuse:w:$EFUSE:m \
  -U lock:w:0xCF:m
