############################################################################
# User-definable settings:

OS = linux

ifeq ($(OS), linux)
AVR_TOOLS_PATH = /usr/bin
AVRDUDE_CONF = /etc/avrdude.conf
endif

ifeq ($(OS), osx)
AVR_PATH = /Applications/Arduino-1.6.0.app/Contents/Resources/Java/hardware/tools/avr
#AVR_PATH = /Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr
#AVR_PATH = /usr/local/CrossPack-AVR
AVR_TOOLS_PATH = $(AVR_PATH)/bin
AVRDUDE_CONF = $(AVR_PATH)/etc/avrdude.conf
endif

AVRDUDE_PROGRAMMER = avrisp2
AVRDUDE_PORT = /dev/tty.usbmodem00028961
UPLOAD_RATE = 115200
MCU = atmega328p
F_CPU = 16000000L

############################################################################
# Variables:

# Git version
GIT_VERSION := $(shell git describe --abbrev=6 --dirty --always --tags)

# Output Intel HEX files
FORMAT = ihex

# Put build deliverables here
BUILD_DIR = $(ROOT)/build

# Programs
CC			= $(AVR_TOOLS_PATH)/avr-gcc
CXX			= $(AVR_TOOLS_PATH)/avr-g++
OBJCOPY		= $(AVR_TOOLS_PATH)/avr-objcopy
OBJDUMP		= $(AVR_TOOLS_PATH)/avr-objdump
AR			= $(AVR_TOOLS_PATH)/avr-ar
SIZE		= $(AVR_TOOLS_PATH)/avr-size
NM			= $(AVR_TOOLS_PATH)/avr-nm
AVRDUDE 	= $(AVR_TOOLS_PATH)/avrdude
REMOVE		= rm -f
MV			= mv -f

# Flags
COMMON_FLAGS 	= -DF_CPU=$(F_CPU) -I. -I$(ROOT)/include -Os -Wall \
			-ffunction-sections -fdata-sections -mmcu=$(MCU) \
			-DARDUINO=$(ARDUINO)
CFLAGS 			= -gstabs $(COMMON_FLAGS) -DGIT_VERSION=\"$(GIT_VERSION)\" # -Wstrict-prototypes # -Wa,-adhlns=$(<:.c=.lst),-gstabs
CXXFLAGS		= -gstabs $(COMMON_FLAGS) -fno-exceptions # -Wa,-adhlns=$(<:.cpp=.lst),-gstabs
ASFLAGS			= -Wa,-adhlns=$(<:.S=.lst),-gstabs -mmcu=$(MCU) -x assembler-with-cpp
LD_FLAGS		= -gstabs -Os -Wl,--gc-sections -mmcu=$(MCU) -Wl,--section-start=.text=$(TEXT_SECTION)
SYSTEM_LIBS		= # -lm
AVRDUDE_FLAGS 	= -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) \
								-b $(UPLOAD_RATE) -C $(AVRDUDE_CONF)
HEXSIZE_FLAGS	= --target=$(FORMAT) $(BUILD_DIR)/$(TARGET).hex
ELFSIZE_FLAGS	= $(BUILD_DIR)/$(TARGET).elf
OBJCOPY_FLAGS = --debugging \
					--change-section-address .data-0x800000 \
					--change-section-address .bss-0x800000 \
					--change-section-address .noinit-0x800000 \
					--change-section-address .eeprom-0x810000 

# Define all object files.
OBJ = $(SRC:.c=.o) $(CXXSRC:.cpp=.o) $(ASRC:.S=.o) 

# Define all listing files.
LST = $(ASRC:.S=.lst) $(CXXSRC:.cpp=.lst) $(SRC:.c=.lst)

# Dependencies
DEPENDS = $(CXXSRC:.cpp=.d) $(SRC:.c=.d)


############################################################################
# Targets:

# Phony targets for various output files
typical: hex sizebefore sizeafter
elf: $(BUILD_DIR)/$(TARGET).elf
hex: $(BUILD_DIR)/$(TARGET).hex
eep: $(BUILD_DIR)/$(TARGET).eep
lss: $(BUILD_DIR)/$(TARGET).lss 
sym: $(BUILD_DIR)/$(TARGET).sym
a: $(BUILD_DIR)/$(TARGET).a
coff: $(BUILD_DIR)/$(TARGET).coff
extcoff: $(BUILD_DIR)/$(TARGET).extcoff

# Create build dir if it does not exist
$(BUILD_DIR):
	test -d $(BUILD_DIR) || mkdir $(BUILD_DIR)

# Link: create ELF output file from objects
$(BUILD_DIR)/$(TARGET).elf: $(BUILD_DIR) $(OBJ) $(LIBS)
	$(CC) $(LD_FLAGS) -o $@ $(OBJ) $(LIBS) $(SYSTEM_LIBS)

# Create library from objects
$(BUILD_DIR)/$(TARGET).a: $(BUILD_DIR) $(OBJ)
	$(AR) rcs $@ $(OBJ)

# Program the device
install: upload
flash: upload
upload: $(BUILD_DIR)/$(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
	-U flash:w:$(BUILD_DIR)/$(TARGET).hex

isp: $(BUILD_DIR)/$(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
	-U lock:w:0x3f:m \
	-U flash:w:$(BUILD_DIR)/$(TARGET).hex \
	-U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m \
	-U lock:w:0x0f:m

dump:
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
	-U flash:r:dump.bin:r

# Display elf size
sizebefore: $(BUILD_DIR)/$(TARGET).elf
	@echo "Size of $< (text=code, data=data, bss=uninitialized vars)"; $(SIZE) $(ELFSIZE_FLAGS)

# Display hex size
sizeafter: $(BUILD_DIR)/$(TARGET).hex
	@echo "Size of $< (data=size of code+data uploaded to the AVR)"; $(SIZE) $(HEXSIZE_FLAGS)

# Clean project.
clean:
	$(REMOVE) $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).eep \
		$(BUILD_DIR)/$(TARGET).cof $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).a \
		$(BUILD_DIR)/$(TARGET).map $(BUILD_DIR)/$(TARGET).sym $(BUILD_DIR)/$(TARGET).lss \
		$(OBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d) $(CXXSRC:.cpp=.s) $(CXXSRC:.cpp=.d)

# Targets that don't produce any file
.PHONY:	typical elf hex eep lss sym program coff extcoff clean sizebefore sizeafter dump


############################################################################
# Pattern rules

# Create the Intel HEX file
%.hex: %.elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

# Create the eeprom contents file
%.eep: %.elf
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create .coff
%.coff: %.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) -O coff-avr $< $@

# Create .extcoff
%.extcoff: %.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) -O coff-ext-avr $< $@

# Create extended listing file from ELF output file.
%.lss: %.elf
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	$(NM) -n $< > $@

# Compile: create object files from C++ source files.
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@ 

# Compile: create object files from C source files.
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@ 

# Compile: create assembler files from C source files.
%.s: %.c
	$(CC) -S $(CFLAGS) $< -o $@

# Assemble: create object files from assembler source files.
%.o: %.S
	$(CC) -c $(ASFLAGS) $< -o $@

# Automatic dependencies
%.d: %.c
	$(CC) -M $(CFLAGS) $< | sed "s;$(notdir $*).o:;$*.o $*.d:;" > $@

%.d: %.cpp
	$(CXX) -M $(CXXFLAGS) $< | sed "s;$(notdir $*).o:;$*.o $*.d:;" > $@

include $(DEPENDS)
