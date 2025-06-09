# Toolchain
CC      := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
LD      := arm-none-eabi-ld

# Teensy flasher
TEENSY  := teensy_loader_cli --mcu=IMXRT1062

# SDK root (adjust to your clone location)
SDK_ROOT := ~/mcuxpressosdk/mcuxpresso-sdk/mcuxsdk/devices

# CPU and optimization flags
CPU_FLAGS := -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -O2

# Include paths: your headers + CMSIS + device + drivers
INCLUDES := \
  -Iinclude \
  -I$(SDK_ROOT)/\

SRCS := \
  src/main.c \
  src/canfd.c \
	src/dsp.c \
	src/adc.c \
	src/mulitilateration.c
# Combine
CFLAGS := $(CPU_FLAGS) $(INCLUDES) -ffunction-sections -fdata-sections

# Use your own linker script
LDSCRIPT := linker.ld

LDFLAGS := \
  -T$(LDSCRIPT) \
  -Wl,--gc-sections \
  -nostdlib

# Default target
all: teensy4.bin

# 1) Link .elf
teensy4.elf: $(OBJS) $(LDSCRIPT)
	$(CC) $(CPU_FLAGS) $(LDFLAGS) -o $@ $(OBJS)

# 2) Convert to raw binary and flash
teensy4.bin: teensy4.elf
	$(OBJCOPY) -O binary $< $@
	$(TEENSY) $@

# 3) Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 4) Clean intermediates
clean:
	rm -f $(OBJS) teensy4.elf teensy4.bin

.PHONY: all clean
