CXX = g++
CXXFLAGS = -std=c++11 -I SDL2/include/SDL2
LDFLAGS = -lSDL2

SRC = src/main.cpp src/chip8.cpp
OBJ = $(SRC:.cpp=.o)

TARGET = chip8_emulator

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)