# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17 -I/home/thomas/acoustics_ws/src/vortex-acoustics/Resources/

# Source files
SRC = main.cpp corelation.cpp tdoa.cpp

# Header files
HEADERS = corelation.h tdoa.h arm_math.h

# Output executable
TARGET = a.out

# Build the project
$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Clean build files
clean:
	rm -f $(TARGET)
