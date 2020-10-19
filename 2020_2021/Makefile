
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

# Another source could also be 
# http://wiki.wlug.org.nz/MakefileHowto

# Wouldn't study at NTNU if I didn't "kok" it


# Need to update this to also include the C-files

CXX      := -c++
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror
LDFLAGS  := -L/usr/lib -lstdc++ -lm
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := program
INCLUDE  := -Iinclude/ -Ialglib/src
SRC      :=                      \
   $(wildcard alglib/src/*.cpp) \
   $(wildcard src/*.cpp) \

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
   @mkdir -p $(@D)
   $(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ $(LDFLAGS)

$(APP_DIR)/$(TARGET): $(OBJECTS)
   @mkdir -p $(@D)
   $(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

.PHONY: all build clean debug release

build:
   @mkdir -p $(APP_DIR)
   @mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
   -@rm -rvf $(OBJ_DIR)/*
   -@rm -rvf $(APP_DIR)/*