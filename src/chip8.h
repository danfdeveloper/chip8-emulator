
#include <cstdint>

const unsigned int MEMORY_SIZE = 4096;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class Chip8
{
public:
	void loadROM(char const *filename);
	void emulateCycle();
	void initialize();
	Chip8()
	{
		pc = 0x200; // Program counter starts at 0x200
	}

	uint8_t V[16]{}; // 16 registers (8 bits each) - VF is the carry flag

	// 0x000-0x1FF - CHIP-8 interpreter
	// 0x050-0x0A0 - Used for font set
	// 0x200-0xFFF - Program ROM and work RAM
	uint8_t memory[MEMORY_SIZE]{}; // 4096 bytes of memory

	uint16_t opcode;	  // Opcode (16 bits)
	uint16_t I;			  // Index register
	uint16_t pc;		  // Program counter
	uint16_t stack[16]{}; // Stack with 16 levels
	uint8_t sp;			  // Stack pointer

	uint8_t gfx[VIDEO_WIDTH * VIDEO_HEIGHT]{}; // 64 x 32 pixels
	uint8_t soundTimer;						   // Sound timer - When above 0, both of these timers count down at 60 Hz. Will play a sound when sound timer reaches 0.
	uint8_t delayTimer;						   // Delay timer

	uint8_t keypad[16]{}; // Keypad with 16 keys (hexidecimal 0-F)
};
