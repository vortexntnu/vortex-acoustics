SOURCES := DSP.c fourier.c hydrophones.c triliteration.c

SOURCE_DIR := src
#BUILD_DIR := build
DOX_DIR := dox

OBJ := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES))

OUT := vortex_acoustics

#DRIVER_ARCHIVE := $(BUILD_DIR)/libdriver.a
#DRIVER_SOURCE := hardware.c io.c

CC := gcc
# CFLAGS := -O0 -g3 -Wall -Werror -std=c11 -I$(SOURCE_DIR)
CFLAGS := -O0 -g3 -Wall -Wno-unused-variable -Wno-switch -std=c11 -I$(SOURCE_DIR)
LDFLAGS := -L$(BUILD_DIR) -ldriver -lcomedi

.DEFAULT_GOAL := $(OUT)

vortex_acoustics : $(OBJ) #| $(DRIVER_ARCHIVE)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR) :
	mkdir -p $@/src

#$(BUILD_DIR)/%.o : $(SOURCE_DIR)/%.c | $(BUILD_DIR)
#	$(CC) $(CFLAGS) -c $< -o $@

#$(BUILD_DIR)/src/%.o : $(SOURCE_DIR)/src/%.c | $(BUILD_DIR)
#	$(CC) $(CFLAGS) -c $< -o $@

#$(DRIVER_ARCHIVE) : $(DRIVER_SOURCE:%.c=$(BUILD_DIR)/driver/%.o)
#	ar rcs $@ $^

.PHONY: clean clean_dox
clean :
	rm -rf $(BUILD_DIR) $(OUT)

clean_dox:
	rm -rf $(DOX_DIR)