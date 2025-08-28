#include "chip8.h"
#include <fstream>
#include <iostream> //input and output
#include <cstdint>	//integers
#include <ctime>	//time

Chip8 myChip8;
uint8_t START_ADDRESS = 0x200;

void decodeOpcode(uint16_t opcode)
{
	switch (opcode)
	{
		// add opcodes here

	default:
		std::cout << "Opcode: " << opcode << " not recognized" << std::endl;
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
		--soundTimer;
	}
	if (delayTimer > 0)
	{
		--delayTimer;
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
}
