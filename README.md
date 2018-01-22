 ![logo](logo/hexloader.png)

# Hexloader

This is a bootloader for the Atmega328p that can flash the program memory from an .hex file pasted on a standard serial terminal like putty. It supports verification and validation of the hex data. It is also FAST over high latency links like the cheap HC-06 bluetooth UART modules.

## Example

Open up a terminal emulator:

	AVR Hexloader 1.0
	Paste your hex file, 'h' for help
	>:

Paste an .hex file:

	Flashed: 2464 OK! (629ms)
	Paste again to verify
	>:

Then paste again to verify:

	Verified: 2464 OK! (616ms)
	Have a nice day!

Your app boots automatically:

	Hello world!

At the prompt, `h` gives some help:

	>: h
	 q      reboot to app
	 r      reboot to bootloader
	 d      dump flash in hex format
	 esc    abort current command

`d` dumps the contents of the flash in ihex format:

	>: d
	:100000000C945D000C9485000C9485000C94850084
	:100010000C9485000C9485000C9485000C9485004C
	:100020000C9485000C9485000C9485000C9485003C
	...
	:00000001FF

And `q`/`r` reboot into app/bootloader.

## Features

 * Takes an intel hex (.hex) file pasted directly over a serial terminal.
 * Baud rates up to 115200 @ 16MHz.
 * Flash verification and hex data validation (checksum and address consistency).
 * Reset handling based on Ralph Doncaster's picoboot (read below)
 * Hands-free booting into bootloader via watchdog timer.
 * 'Breathing LED' visual cue that the bootloader is running.

## Background

The motivation behind this is to overcome a few problems I've had with the standard Arduino bootloader (Optiboot) when used with an HC-06 bluetooth UART:

First, stk500 based bootloaders are *much* slower over bluetooth than over plain USB. The reason is that avrdude expects an ack after every command before moving on to the next. I measured the latency of the bluetooth module to be around 30ms roundtrip. If you run avrdude in super-verbose mode with -v -v -v -v, you will see that flashing or reading every page takes two commands (first the address, then the data), so 60ms are wasted every 128 bytes. That's in addition to the transmission itself and the actual flashing time. Overall, avrdude takes 16 seconds to flash 20KB and then another 14 seconds to read it back for verification, so total 30 seconds. It might not seem much for an one-off run, but if you are developing and uploading repeatedly, it becomes painfully slow.

Another problem I found is that the HC-06 doesn't have a DTR line, so the standard behavior of Arduino where DTR resets the MCU into the bootloader doesn't work. You need to do the reset manually within the bootloader timeout window, which is not always easy to get right and becomes another source of frustration. Plus, it requires physically reaching out to the reset button.

## Booting into the bootloader

Pressing the reset button switches between the bootloader and the app. Thus, to reboot your app you would have to press reset twice. On power up, the bootloader will jump straight to the application. See the technical details in the "How it works" section.

The bootloader can also be invoked from your application by calling this function:

```c
#include <avr/wdt.h>

void reset() 
{
    cli();
    asm volatile ("mov r2, 0");
    wdt_enable(WDTO_15MS);
    for (;;);
}
```

The asm instruccion `mov r2, 0` ensures that the 'boot to app' signature is not found and the bootloader doesn't boot back into the application.

## How to compile and flash

### Windows pre-requisites

You need:

 * [Arduino](https://www.arduino.cc/en/main/software) (Windows installer)
 * [Git](https://git-scm.com/download/win)
 * [GNU Coreutils](http://gnuwin32.sourceforge.net/packages/coreutils.htm) (Setup program, complete package without source code)
 * [GNU Make](http://gnuwin32.sourceforge.net/packages/make.htm) (Setup program, complete package without source code)
 * [GNU Sed](http://gnuwin32.sourceforge.net/packages/sed.htm) (Setup program, complete package without source code)

Add `C:\Program Files (x86)\GnuWin32\bin` to the PATH environment variable. The git installer should take care of that itself.

To modify the environment go to Cortana -> environ, or in Windows <= 7 go to Control Panel and use the search box to search for "environment".

### Linux pre-requisites

```
sudo apt-get install avrdude gcc-avr avr-libc make
```

### MacOS pre-requisites

Install the Xcode command line tools:

```
xcode-select --install
```

Install Arduino.

### Compiling and uploading

In `Makefile.mk`, set the `OS` variable to `windows`, `linux` or `osx` depending on your host platform. Check `AVR_PATH` and `AVR_TOOLS_PATH`. In `hexloader/Makefile` set the variables `AVRDUDE_PROGRAMMER` and `AVRDUDE_PORT`.

Then:

```
make isp
```

This will compile and try to flash using the programmer defined in `AVRDUDE_PROGRAMMER` (avrisp2 by default).

There is also a precompiled version under hexloader/build.


## How it works

Hexloader works by taking an Intel Hex file sent over the serial port. Hex files are the compilation output of Arduino or avr-gcc + objcopy. Pasting it directly on a terminal emulator flashes the chip. You can use any terminal like putty, screen, cu, hyperterminal, etc.

Internally, a FIFO buffer queues up data as it comes in. Every line in the hex file is checksummed, plus there are other consistency checks like making sure addresses start at 0 and are monotonically increasing. For example, if you accidentally paste an incomplete hex that begins at address 0x0070 instead of 0x0000 you will see something like:

```
First address must be 0:
:10007000726C642C20707265737320277227207451
   ^^^^
Rebooting into bootloader

AVR Hexloader 1.0.ae2dc8-dirty
Paste your hex file, 'h' for help
>:
```

Same if the checksum fails:

```
Checksum error in line:
:100000000C9446000C9458000C9458000C94580020
Rebooting into bootloader
```

After receiving 128 bytes, the current page is erased and flashed while more data for the next page queues up.

### Flow control

This all works without any kind of flow control as long as data comes in at a slower pace than it is flashed by the AVR. Flashing or erasing a 128 bytes page on an atmega328p takes a maximum 4.5ms according to the datasheet (chapter 26.2.4), so that's 9ms for a full erase + program cycle. On the other hand, 128 bytes take 352 characters. As long as 352 bytes (3520 bits with 8,N,1 frames) take more than 9ms, they won't overrun the fifo. That gives a theoretical maximum baud rate of 390 Kbps.

In practical terms, I've tested at 230.4 Kbps but that results in rx errors. This might be caused by 230400 not being an exact divisor of the CPU frequency (16 MHz), which results in some clock skew. Perhaps using a clock that is multiple of 230400, like 14.7456 MHz would work without issues.

### Reset handling

The way this is implemented is inspired by Ralph Doncaster's picoboot:

  1. On powerup, the bootloader goes straight into the app (this is done by checking that both EXTRF and WDRF in MCUSR are 0).
  2. After pressing reset, the bootloader checks a special signature `0xb0aa` in registers r2 and r3. If r2 == 0xb0 and r3 == 0xaa, then it will jump into the app, otherwise it stays in the bootloader. Since it's unlikely (p = 1/65536) that r2 and r3 contain those exact values at any random point, the bootloader will run.
  3. The bootloader sets r2 and r3 to the bootloader signature `0xb0aa` and never uses those registers. So when we press reset again, we'll be back to the app.

### Limitations and porting to other platforms

Since the watchdog is used to reboot into the bootloader, it can't be used to recover the application from lockups. A timeout mechanism should be implemented in the bootloader in case of inactivity, which wouldn't be too hard. Besides that, the watchdog is disabled by the bootloader at the beginning of the code, so the application doesn't need to disable it itself.

Hexloader takes the whole NRWW program space (upper 4KB), leaving 28 KB for the application. Program memory (32 KB) in the atmega328p is divided in two blocks, RWW (read while write) and NRWW (no read while write). RWW can be flashed while the CPU does other things like serving UART interrupts. However, programming the NRWW halts the CPU and without flow control all the pasted data received after a CPU halt would be lost.

Also a hardware UART capable of rx/tx interrupts is required so that reading serial data and flashing can happen concurrently.

