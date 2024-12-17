CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra $(shell sdl2-config --cflags)
LIBS = $(shell sdl2-config --libs)
SRC = src/main.cpp src/game.cpp src/ship.cpp src/asteroid.cpp src/bullet.cpp src/vector2d.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = Asteroids

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
