# Compiler
CXX = g++

# Include paths (modify the Eigen path as needed)
INCLUDES = -I/usr/include/eigen3

# Compiler Flags
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 $(INCLUDES)

# Source files
SRCS = main.cpp cross.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = Thomas_impl_2

# Default rule
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

# Rebuild rule
rebuild: clean all
