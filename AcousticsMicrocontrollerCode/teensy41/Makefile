
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
SDK_ROOT := $(HOME)/mcuxpressosdk/mcuxpresso-sdk/mcuxsdk/devices/RT/RT1060/MIMXRT1062
PERIPH := $(HOME)/mcuxpressosdk/mcuxpresso-sdk/mcuxsdk/devices/RT/RT1060/periph
CMSIS :=  $(HOME)/mcuxpressosdk/mcuxpresso-sdk/mcuxsdk/arch/arm/CMSIS
CMSIS_DSP_SRC_DIR := $(CMSIS)/DSP/Source

DSP_BASIC_MATH    := $(CMSIS_DSP_SRC_DIR)/BasicMathFunctions/BasicMathFunctions.c
DSP_COMPLEX       := $(CMSIS_DSP_SRC_DIR)/ComplexMathFunctions/ComplexMathFunctions.c
DSP_CONTROLLER    := $(CMSIS_DSP_SRC_DIR)/ControllerFunctions/ControllerFunctions.c
DSP_FAST          := $(CMSIS_DSP_SRC_DIR)/FastMathFunctions/FastMathFunctions.c
DSP_FILTER        := $(CMSIS_DSP_SRC_DIR)/FilteringFunctions/FilteringFunctions.c
DSP_SUPPORT       := $(CMSIS_DSP_SRC_DIR)/SupportFunctions/SupportFunctions.c
DSP_STATS         := $(CMSIS_DSP_SRC_DIR)/StatisticsFunctions/StatisticsFunctions.c
DSP_TRANSFORM     := $(CMSIS_DSP_SRC_DIR)/TransformFunctions/TransformFunctions.c
DSP_MATRIX        := $(CMSIS_DSP_SRC_DIR)/MatrixFunctions/MatrixFunctions.c
DSP_COMMON_TABLES := $(CMSIS_DSP_SRC_DIR)/CommonTables/CommonTables.c



####################################
# CPU and flags
####################################
CPU_FLAGS := -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -O2 -g -DCPU_MIMXRT1062DVJ6B -D__START=main

# Include paths: your own headers first, then CMSIS/device/driver headers
INCLUDES := \
    -Iinclude \
    -I$(SDK_ROOT)/\
    -I$(PERIPH)/\
    -I$(CMSIS)/Core/Include\
    -I$(CMSIS)/DSP/Include \
    -I$(CMSIS)/DSP/PrivateInclude

CFLAGS := $(CPU_FLAGS) $(INCLUDES) -ffunction-sections -fdata-sections

####################################
# Linker
####################################
# Point to your adapted linker script (e.g. linker.ld in project root)
LDSCRIPT := $(SDK_ROOT)/gcc/MIMXRT1062xxxxx_flexspi_nor_sdram.ld

LDFLAGS := \
    -T$(LDSCRIPT) \
    -Wl,--gc-sections 

####################################
# Sources and objects
####################################
# List your C sources here


DSP_SRC := \
    $(DSP_BASIC_MATH) \
    $(DSP_COMPLEX) \
    $(DSP_CONTROLLER) \
    $(DSP_FAST) \
    $(DSP_FILTER) \
    $(DSP_SUPPORT) \
    $(DSP_STATS) \
    $(DSP_TRANSFORM) \
		$(DSP_MATRIX) \
		$(DSP_COMMON_TABLES)


DSP_SRC := $(sort $(DSP_SRC))


$(info Final DSP_SRC files:=$(DSP_SRC))

C_SRCS := \
    src/main.c \
    src/dsp.c \
    src/adc.c \
    src/pit.c \
    src/clock.c \
    src/multilateration.c \
		src/can_fd.c \
		src/usart.c \
		$(SDK_ROOT)/system_MIMXRT1062.c \
		$(DSP_SRC)

# List your assembly startup file (ensure correct path)
ASM_SRCS := \
    $(SDK_ROOT)/gcc/startup_MIMXRT1062.S

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
