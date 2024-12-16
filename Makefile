# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17

# SDL2 Flags
SDL2_CFLAGS := $(shell sdl2-config --cflags)
SDL2_LDFLAGS := $(shell sdl2-config --libs)

# Target executable
TARGET = ast
SRC = main.cpp

# Build rule
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SDL2_CFLAGS) $(SRC) -o $(TARGET) $(SDL2_LDFLAGS)

# Clean rule
clean:
	rm -f $(TARGET)
