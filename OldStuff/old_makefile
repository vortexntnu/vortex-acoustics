
#SOURCES := DSP.c fourier.c hydrophones.c triliteration.c

#SOURCE_DIR := src
#BUILD_DIR := build
#DOX_DIR := dox

#OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))

#OUT := vortex_acoustics

#DRIVER_ARCHIVE := $(BUILD_DIR)/libdriver.a
#DRIVER_SOURCE := hardware.c io.c

#CC := gcc
# CFLAGS := -O0 -g3 -Wall -Werror -std=c11 -I$(SOURCE_DIR)
#CFLAGS := -O0 -g3 -Wall -Wno-unused-variable -Wno-switch -std=c11 -I$(SOURCE_DIR)
#LDFLAGS := -L$(BUILD_DIR) -ldriver -lcomedi

#.DEFAULT_GOAL := $(OUT)

#vortex_acoustics : $(OBJ) #| $(DRIVER_ARCHIVE)
#	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

#$(BUILD_DIR) :
#	mkdir -p $@/src

#$(BUILD_DIR)/%.o : $(SOURCE_DIR)/%.c | $(BUILD_DIR)
#	$(CC) $(CFLAGS) -c $< -o $@

#$(BUILD_DIR)/src/%.o : $(SOURCE_DIR)/src/%.c | $(BUILD_DIR)
#	$(CC) $(CFLAGS) -c $< -o $@

#$(DRIVER_ARCHIVE) : $(DRIVER_SOURCE:%.c=$(BUILD_DIR)/driver/%.o)
#	ar rcs $@ $^

#.PHONY: clean clean_dox
#clean :
#	rm -rf $(BUILD_DIR) $(OUT)

#clean_dox:
#	rm -rf $(DOX_DIR)





# The makefile is basicly copy-pasted from
# https://www.partow.net/programming/makefile/index.html

# CXX      := -c++
CXX      := g++  
CC       := gcc

# CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror
CPPFLAGS := -pedantic-errors -Wall -Wextra -Werror

# LDFLAGS  := -L/usr/lib -lstdc++ -lm
LDFLAGS     := -lm
BUILD       := ./build
OBJ_DIR     := $(BUILD)/objects
APP_DIR     := $(BUILD)/apps
TARGET      := program

CINCLUDE :=
   -IResource/STM32F7xx_HAL_Driver/Src 
   -IResource/CMSIS/DSP_Lib/Source/BasicMathFunctions
   -IResource/CMSIS/DSP_Lib/Source/CommonTables
   -IResource/CMSIS/DSP_Lib/Source/ComplexMathFunctions
   -IResource/CMSIS/DSP_Lib/Source/ControllerFunctions
   -IResource/CMSIS/DSP_Lib/Source/FastMathFunctions
   -IResource/CMSIS/DSP_Lib/Source/FilteringFunctions
   -IResource/CMSIS/DSP_Lib/Source/MatrixFunctions
   -IResource/CMSIS/DSP_Lib/Source/StatisticsFunctions
   -IResource/CMSIS/DSP_Lib/Source/SupportFunctions
   -IResource/CMSIS/DSP_Lib/Source/TransformFunctions

CXXINCLUDE  :=
   -ISource

CSRC        :=                      \
   $(wildcard Resource/STM32F7xx_HAL_Driver/Src/*.c) \
   $(wildcard Resource/CMSIS/DSP_Lib/Source/BasicMathFunctions/*.c) \
   $(wildcard Resource/CMSIS/DSP_Lib/Source/CommonTables/*.c) \
   $(wildcard Resource/CMSIS/DSP_Lib/Source/ComplexMathFunctions/*.c) \
   $(wildcard Resource/CMSIS/DSP_Lib/Source/ControllerFunctions/*.c) \
   $(wildcard Resource/CMSIS/DSP_Lib/Source/FastMathFunctions/*.c) \
   $(wildcard Resource/CMSIS/DSP_Lib/Source/FilteringFunctions/*.c) \
   $(wildcard Resource/CMSIS/DSP_Lib/Source/MatrixFunctions/*.c) \
   $(wildcard Resource/CMSIS/DSP_Lib/Source/StatisticsFunctions/*.c) \
   $(wildcard Resource/CMSIS/DSP_Lib/Source/SupportFunctions/*.c) \
   $(wildcard Resource/CMSIS/DSP_Lib/Source/TransformFunctions/*.c) \

CXXSRC      :=
   $(wildcard Source/*.cpp) \

COBJ        := $(CSRC:%.c=$(OBJ_DIR)/%.o)
CXXOBJ      := $(CXXSRC:%.cpp=$(OBJ_DIR)/%.o)


all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.c
   @mkdir -p $(@D)
   $(CC) $(CPPFLAGS) $(CINCLUDE) -c $< -o $@ $(LDFLAGS)

$(APP_DIR)/$(TARGET): $(OBJECTS)
   @mkdir -p $(@D)
   $(CC) $(CPPFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cpp
   @mkdir -p $(@D)
   $(CXX) $(CXXFLAGS) $(CXXINCLUDE) -c $< -o $@ $(LDFLAGS)

$(APP_DIR)/$(TARGET): $(OBJECTS)
   @mkdir -p $(@D)
   $(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

.PHONY: all build clean debug release

build:
   @mkdir -p $(APP_DIR)
   @mkdir -p $(OBJ_DIR)

debug: CPPFLAGS += -DDEBUG -g
debug: all

release: CPPFLAGS += -O2
release: all

clean:
   -@rm -rvf $(OBJ_DIR)/*.o
   -@rm -rvf $(APP_DIR)/*.o