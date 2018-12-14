#include "CPU.h"


CPU::CPU()
{}

CPU::CPU(Memory* RAM, SDLAPI* peripherals)
	:m_RAM(RAM), m_Peripherals(peripherals)
{}

CPU::~CPU()
{
}

void CPU::UsePeripherals(SDLAPI* peripherals)
{
	m_Peripherals = peripherals;
}

void CPU::UseMemory(Memory* RAM)
{
	m_RAM = RAM;
}
