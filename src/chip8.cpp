#include "chip8.h"
#include <fstream>	//files
#include <iostream> //input and output
#include <cstdint>	//integers
#include <ctime>	//time
#include <cstdlib>	//for rand()

uint16_t START_ADDRESS = 0x200;

void Chip8::decodeOpcode(uint16_t opcode)
{
	// register indeces
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	// 4 bit immediate or nibble
	uint8_t n = opcode & 0x000F;
	// 8 bit immediate
	uint8_t nn = opcode & 0x00FF;
	// address
	uint16_t nnn = opcode & 0x0FFF;

	// execute opcode
	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode)
		{
		case 0x00E0: // clear screen
			for (int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; ++i)
				gfx[i] = 0;
			break;

		case 0x00EE: // return from subroutine
			sp--;
			pc = stack[sp];
			break;

		default:
			std::cout << "Unknown opcode: " << std::hex << opcode << std::endl;
			break;
		}
		break;

	case 0x1000: // jump to address nnn
		pc = nnn;
		break;

	case 0x2000: // call subroutine at address nnn
		stack[sp] = pc;
		sp++;
		pc = nnn;
		break;

	case 0x3000: // skip next instruction if Vx = nn
		if (V[x] == nn)
			pc += 2;
		break;

	case 0x4000: // skip next instruction if Vx != nn
		if (V[x] != nn)
			pc += 2;
		break;

	case 0x5000: // skip next instruction if Vx = Vy
		if (V[x] == V[y])
			pc += 2;
		break;

	case 0x6000: // load - set Vx = nn
		V[x] = nn;
		break;

	case 0x7000: // set Vx = Vx + nn
		V[x] += nn;
		break;

	case 0x8000:
		switch (n)
		{
		case 0x0: // set Vx = Vy
			V[x] = V[y];
			break;

		case 0x1: // set Vx = Vx OR Vy
			V[x] |= V[y];
			break;

		case 0x2: // set Vx = Vx AND Vy
			V[x] &= V[y];
			break;

		case 0x3: // set Vx = Vx XOR Vy
			V[x] ^= V[y];
			break;

		case 0x4: // set Vx = Vx + Vy, set VF = carry if need be
		{
			uint16_t sum = V[x] + V[y];
			V[0xF] = sum > 255 ? 1 : 0;
			V[x] = sum & 0xFF;
		}
		break;

		case 0x5: // set Vx = Vx - Vy, set VF = NOT borrow
			V[0xF] = V[x] > V[y] ? 1 : 0;
			V[x] -= V[y];
			break;

		case 0x6: // set Vx = Vx SHR 1
			V[0xF] = V[x] & 0x1;
			V[x] >>= 1;
			break;

		case 0x7: // set Vx = Vy - Vx, set VF = NOT borrow
			V[0xF] = V[y] > V[x] ? 1 : 0;
			V[x] = V[y] - V[x];
			break;

		case 0xE: // set Vx = Vx SHL 1
			V[0xF] = (V[x] & 0x80) >> 7;
			V[x] <<= 1;
			break;

		default:
			std::cout << "Unknown opcode: " << std::hex << opcode << std::endl;
			break;
		}
		break;

	case 0x9000: // skip next instruction if Vx != Vy
		if (V[x] != V[y])
			pc += 2;
		break;

	case 0xA000: // set I = nnn
		I = nnn;
		break;

	case 0xB000: // jump to location nnn + V0
		pc = nnn + V[0];
		break;

	case 0xC000: // set Vx = random byte AND nn
		V[x] = (rand() % 256) & nn;
		break;

	case 0xD000: // display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
	{
		uint8_t xPos = V[x] % VIDEO_WIDTH;
		uint8_t yPos = V[y] % VIDEO_HEIGHT;

		V[0xF] = 0;

		for (int row = 0; row < n; ++row)
		{
			uint8_t spriteByte = memory[I + row];

			for (int col = 0; col < 8; ++col)
			{
				uint8_t spritePixel = spriteByte & (0x80 >> col);
				int screenPixel = (yPos + row) * VIDEO_WIDTH + (xPos + col);

				if (spritePixel)
				{
					if (gfx[screenPixel] == 1)
						V[0xF] = 1;

					gfx[screenPixel] ^= 1;
				}
			}
		}
	}
	break;

	case 0xE000:
		switch (nn)
		{
		case 0x9E: // skip next instruction if key with the value of Vx is pressed
			if (keypad[V[x]])
				pc += 2;
			break;

		case 0xA1: // skip next instruction if key with the value of Vx is NOT pressed
			if (!keypad[V[x]])
				pc += 2;
			break;

		default:
			std::cout << "Unknown opcode: " << std::hex << opcode << std::endl;
			break;
		}
		break;

	case 0xF000:
		switch (nn)
		{
		case 0x07: // set Vx = delay timer value
			V[x] = delayTimer;
			break;

		case 0x0A: // wait for a key press, then store the value of the key in Vx
		{
			bool keyPressed = false;
			for (int i = 0; i < 16; ++i)
			{
				if (keypad[i])
				{
					V[x] = i;
					keyPressed = true;
					break;
				}
			}

			if (!keyPressed)
				pc -= 2; // repeat the instruction
		}
		break;

		case 0x15: // set delay timer = Vx
			delayTimer = V[x];
			break;

		case 0x18: // set sound timer = Vx
			soundTimer = V[x];
			break;

		case 0x1E: // set I = I + Vx
			I += V[x];
			break;

		case 0x29: // set I = location of sprite for digit Vx
			I = 0x50 + (V[x] * 5);
			break;

		case 0x33: // store BCD representation of Vx in memory locations I, I+1, and I+2
			memory[I] = V[x] / 100;
			memory[I + 1] = (V[x] / 10) % 10;
			memory[I + 2] = V[x] % 10;
			break;

		case 0x55: // store registers V0 through Vx in memory starting at location I
			for (int i = 0; i <= x; ++i)
				memory[I + i] = V[i];
			break;

		case 0x65: // read registers V0 through Vx from memory starting at location I
			for (int i = 0; i <= x; ++i)
				V[i] = memory[I + i];
			break;

		default:
			std::cout << "Unknown opcode: " << std::hex << opcode << std::endl;
			break;
		}
		break;

		default:
			std::cout << "Unknown opcode: " << std::hex << opcode << std::endl;
			break;
	}
}

void Chip8::emulateCycle()
{
	// fetch the opcode
	// since opcode is 16 bits and the memory is based off of bytes, we need to read two bytes back to back and combine them
	opcode = memory[pc] << 8 | memory[pc + 1];
	pc = pc + 2;

	// decode the opcode - this also executes it
	decodeOpcode(opcode);

	if (soundTimer > 0)
	{
		soundTimer--;
	}
	if (delayTimer > 0)
	{
		delayTimer--;
	}
}

void Chip8::loadROM(char const *filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::streampos size = file.tellg();
		char *buffer = new char[size];
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		for (long i = 0; i < size; ++i)
		{
			memory[START_ADDRESS + i] = buffer[i];
		}

		delete[] buffer;
	}
}

void Chip8::initialize()
{
	pc = 0x200;
	opcode = 0;
	I = 0;
	sp = 0;

	// clears the screen
	for (int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; i++)
	{
		gfx[i] = 0;
	}

	// clears stack and registers
	for (int i = 0; i < 16; i++)
	{
		stack[i] = 0;
		V[i] = 0;
	}

	// clears memory
	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		memory[i] = 0;
	}

	// clear keypad
	for (int i = 0; i < 16; i++)
	{
		keypad[i] = 0;
	}

	// initialize fontset
	uint8_t fontset[80] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	// load fontset into memory
	for (int i = 0; i < 80; i++)
	{
		memory[0x50 + i] = fontset[i];
	}

	// reset timers
	delayTimer = 0;
	soundTimer = 0;
}