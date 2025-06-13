
####################################
# Toolchain and paths
####################################
# You may need to adjust CC path if not in PATH
CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
LD      := arm-none-eabi-ld

# Teensy flasher; ensure teensy_loader_cli is installed
TEENSY  := teensy_loader_cli --mcu=IMXRT1062

# SDK root: adjust to where you cloned MCUXpresso SDK
# Use absolute or $(HOME) to avoid '~' expansion issues
SDK_ROOT := $(HOME)/mcuxpressosdk/mcuxpresso-sdk/mcuxsdk/devices/RT/RT1060/MIMXRT1062/
PERIPH := $(HOME)/mcuxpressosdk/mcuxpresso-sdk/mcuxsdk/devices/RT/RT1060/periph/
CMSIS :=  $(HOME)/mcuxpressosdk/mcuxpresso-sdk/mcuxsdk/arch/arm/CMSIS/

####################################
# CPU and flags
####################################
CPU_FLAGS := -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -O2 -g

# Include paths: your own headers first, then CMSIS/device/driver headers
INCLUDES := \
    -Iinclude \
    -I$(SDK_ROOT)/\
    -I$(PERIPH)/\
    -I$(CMSIS)/Core/Include\
    -I$(CMSIS)/DSP/Include

CFLAGS := $(CPU_FLAGS) $(INCLUDES) -ffunction-sections -fdata-sections

####################################
# Linker
####################################
# Point to your adapted linker script (e.g. linker.ld in project root)
LDSCRIPT := linker.ld

LDFLAGS := \
    -T$(LDSCRIPT) \
    -Wl,--gc-sections \
    -nostdlib

####################################
# Sources and objects
####################################
# List your C sources here
C_SRCS := \
    src/main.c \
    src/canfd.c \
    src/dsp.c \
    src/adc.c \
    src/pit.c \
    src/clock.c \
    src/multilateration.c

# List your assembly startup file (ensure correct path)
ASM_SRCS := \
    src/startup_MIMXRT1062.S

# Derive object filenames
C_OBJS   := $(C_SRCS:.c=.o)
ASM_OBJS := $(ASM_SRCS:.S=.o)

OBJS := $(C_OBJS) $(ASM_OBJS)

####################################
# Default target
####################################
all: teensy4.bin

####################################
# Link .elf
####################################
teensy4.elf: $(OBJS) $(LDSCRIPT)
	$(CC) $(CPU_FLAGS) $(LDFLAGS) -o $@ $(OBJS)

####################################
# Convert to binary and flash
####################################
teensy4.bin: teensy4.elf
	$(OBJCOPY) -O binary $< $@
	$(TEENSY) $@

####################################
# Compile C files
####################################
# Pattern rule for .c → .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

####################################
# Assemble .S files
####################################
# Pattern rule for .S → .o
%.o: %.S
	$(CC) $(CPU_FLAGS) -c $< -o $@

####################################
# Clean
####################################
clean:
	rm -f $(OBJS) teensy4.elf teensy4.bin

.PHONY: all clean
