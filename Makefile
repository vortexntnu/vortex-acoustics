BUILD_DIR := build
DOX_DIR := dox

SOURCE_DIR ?= ./Source

DRIVER_SOURCE ?= ./Resource/STM32F7xx_HAL_Driver/Src \
	./Resource/CMSIS/DSP_Lib/Source/BasicMathFunctions \
	./Resource/CMSIS/DSP_Lib/Source/CommonTables \
	./Resource/CMSIS/DSP_Lib/Source/ComplexMathFunctions \
	./Resource/CMSIS/DSP_Lib/Source/ControllerFunctions \
	./Resource/CMSIS/DSP_Lib/Source/FastMathFunctions \
	./Resource/CMSIS/DSP_Lib/Source/FilteringFunctions \
	./Resource/CMSIS/DSP_Lib/Source/MatrixFunctions \
	./Resource/CMSIS/DSP_Lib/Source/StatisticsFunctions \
	./Resource/CMSIS/DSP_Lib/Source/SupportFunctions \
	./Resource/CMSIS/DSP_Lib/Source/TransformFunctions


SOURCES := $(shell find $(SOURCE_DIR) -name *.cpp -or -name *.c)

OBJ := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

OUT := vortex_acoustics

DRIVER_ARCHIVE := $(BUILD_DIR)/libdriver.a
DRIVER_SOURCE := $(shell find $(DRIVER_SRC_DIRS) -name *.cpp -or -name *.c)

CC := gcc
CXX := g++
# CFLAGS := -O0 -g3 -Wall -Werror -std=c11 -I$(SOURCE_DIR)
CFLAGS := -O0 -g3 -Wall -Wno-unused-variable -Wno-switch -std=c11 -I$(SOURCE_DIR)
LDFLAGS := -L$(BUILD_DIR) -ldriver -lcomedi

.DEFAULT_GOAL := $(OUT)

vortex_acoustics : $(OBJ) | $(DRIVER_ARCHIVE)
	$(CXX) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR) :
	mkdir -p $@/driver

$(BUILD_DIR)/%.o : $(SOURCE_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/driver/%.o : $(SOURCE_DIR)/driver/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DRIVER_ARCHIVE) : $(DRIVER_SOURCE:%.c=$(BUILD_DIR)/driver/%.o)
	ar rcs $@ $^

.PHONY: clean clean_dox
clean :
	rm -rf $(BUILD_DIR) $(OUT)

clean_dox:
	rm -rf $(DOX_DIR)