CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g -Iinclude
LDFLAGS = -lSDL2

SRC = src/main.cpp src/chip8.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = chip8

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
