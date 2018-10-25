#pragma once

#include <fstream>
#include <iterator>

#include "SDLAPI.h"
#include "CPU.h"

class VM
{
public:
	explicit VM(CPU* processor, const size_t& RAMSize);
	~VM();

	void MapKeyCodes(const SDLAPI::KeyMap& keymap);
	void Start(const char* filename);
private:
	Memory m_RAM;
	SDLAPI m_Peripherals;
	CPU* m_CPU;
};
