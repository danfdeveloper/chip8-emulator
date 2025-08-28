# Compiler and flags
CXX := g++
CXXFLAGS := -I./SDL2/include      # use if code has #include <SDL2/SDL.h>
#CXXFLAGS := -I./SDL2/include/SDL2 # use if code has #include <SDL.h>

LDFLAGS := -L./SDL2/lib -lmingw32 -lSDL2main -lSDL2

# Targets
SRC := src/m.cpp
OUT := m.exe
DLL := SDL2.dll

all: $(OUT) copy-dll

$(OUT): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) $(LDFLAGS) -o $(OUT)

copy-dll:
	cp ./SDL2/lib/$(DLL) .

clean:
	-rm /f /q $(OUT) $(DLL) 2>nul