# Compiler and Flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

# Target executable
TARGET = build/main

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# Source files
SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/multilateration_simulator.cpp

# Object files
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/multilateration_simulator.o

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS) | $(BUILD_DIR)
	@echo "Linking..."
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Clean up build files
clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR)

# Run the program
run: all
	./$(TARGET)

# Phony targets
.PHONY: all clean run
