# Project settings
PROJECT      := thomas_impl
TARGET       := $(PROJECT)
BUILD_DIR    := _build

# Defime the compiler and linker to be used, compiler is arm gnu g++ here since the code is in C++. Use arm gnu gcc for C code base
# The linker is the same for both C++ and C code so no need to change that. 
# You have to make sure that the compiler is both installed and added to PATH in order for this to work
# If you have the compiler/linker installed but not added to path, change this by providing the exact path to where you compiler and linker is. 
CXX          := arm-none-eabi-g++
OBJCOPY      := arm-none-eabi-objcopy

# Find all .cpp source files in the project folder by running wildcard command
SRC         := $(wildcard *.cpp)
# Create a list of object files in the build directory (e.g., _build/<file_name>.o)
OBJ         := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC))

# External include path to libraries outside of this folder. 
# The path is defined in relative position to this folder. 
# If for any reason the folder structure changes, you will need to make sure that this relative path
# Will still work or change it according to the new changes. 
# If you change it. DONT use absolute path as in ~/home/<user_name>/.....etc since that will only work 
# On your pc alone as others have different user names, might have a different file structure entirely, and can download the project in a different path.
EXTERNAL_INC := -I ../Include

# Compiler flags: optimization, debugging, MCU settings, and include paths
CXXFLAGS    := -O2 -g3 -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 \
               -Wall -Werror -ffunction-sections -fdata-sections $(EXTERNAL_INC)

# Linker flags: keep the same target settings and enable garbage collection of unused sections
LDFLAGS     := -O2 -g3 -mcpu=cortex-m7 -mthumb -Wl,--gc-sections -specs=nosys.specs

# Libraries to link (add -L flags if libraries are in a nonstandard path)
LIBS        := -lc -lnosys -lm

# Default target: build the ELF and then the HEX file.
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex

# Ensure the build directory exists by running the provided command.
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule: compile each .cpp file into an object file in the build directory.
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule: link all object files into the final ELF executable.
$(BUILD_DIR)/$(TARGET).elf: $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

# Rule: create a HEX file from the ELF executable.
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

# Clean up build directory
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean

# In case you have problems with either repurposing or editing this file
# Contact Ibrahim F. Abdalaal for help