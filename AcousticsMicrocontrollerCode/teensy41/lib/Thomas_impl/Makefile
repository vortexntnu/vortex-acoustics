# Define the project name. Used mainly in the end file name
PROJECT = thomas_impl

# Define the compiler to be used. (This compiler is the standard for ARM CORTEX MCUs) 
CC = arm-none-eabi-g++

# Define the path for the h files to be included in the compiling stage
INCLUDE_DIRS = -I ./

# Runs a shell script that would find all available c source files to compile
SRC := $(shell find . -type f -name '*.cpp')


# First part finds all the .h files to be linked. Second part defines the path to the external libraries to be linked (if not defined here then must be defined when including the library itself)
INCLUDE := $(shell find . -type f -name '*.h') -I /home/thomas/acoustics_ws/src/vortex-acoustics/AcousticsMicrocontrollerCode/teensy41/lib/Include

# Defines the compiler flags: The MCU CPU type, mthumb sets the arm architecture to thumb, set the floating point unit, float type, additional compiler flags for optimization, include files
CFLAGS = -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=softfp -Wall -O2 $(INCLUDE_DIRS)

# Defines the linker flagsØ The MCU CPU, mthmb sets the arm architecture to thumb, the include files
LDFLAGS = -mcpu=cortex-m7 -mthumb $(INCLUDE) 

# Define object files as "<name_of_compiled_c_file>.o"
OBJ = $(SRC:.cpp=.o)

# Run compilation; generate the output files to the relevant c files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Generate the ELF (Executable and Linked Format) file from the available/to be linked output files (.o files)
$(PROJECT).elf: $(OBJ)
	$(CC) $(LDFLAGS) $< -o $@ 

compile: $(SRC)
	$(CC) $(CFLAGS) -c $^ -o $(PROJECT).o

all: $(PROJECT).elf

#clean: 
#	rm -f $(OBJ)