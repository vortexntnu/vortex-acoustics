# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -Ilib -Ilib/TheoreticalTDOACalculator

# Directories
SRC_DIR = src
LIB_DIR = lib
BUILD_DIR = build

# Find all .cpp files in src/ and lib/ (recursively)
SRC_FILES = $(shell find $(SRC_DIR) $(LIB_DIR) -name "*.cpp")

# Generate object file names in the build/ directory
OBJ_FILES = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Output binary name
TARGET = main.out

# Default target (builds the project)
all: $(TARGET)

# Compile the main executable
$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile all source files into object files
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	mkdir -p $(@D)  # Create subdirectories if needed
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Run the program after building
run: all
	./$(TARGET)

# Clean up build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
