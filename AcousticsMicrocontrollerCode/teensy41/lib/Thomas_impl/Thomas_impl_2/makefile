# Compiler and flags
CXX       = g++
CXXFLAGS  = -std=c++11 -I. -I./matplotlib-cpp $(shell python3-config --includes)
# Use python3-config to get most linker flags, and add -lpython3.10 if it's not already included.
LDFLAGS   = $(shell python3-config --ldflags) -lpython3.10

# Source files and target
SOURCES         := $(wildcard *.cpp)
OBJECTS   = $(SOURCES:.cpp=.o)
TARGET    = main

all: $(TARGET)

# Linking step
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile step
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)