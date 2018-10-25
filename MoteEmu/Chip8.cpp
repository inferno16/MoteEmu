#include "Chip8.h"



Chip8::Chip8()
{
	m_RNG.seed(std::random_device()());
}


Chip8::~Chip8()
{
	delete m_SP;
}

void Chip8::Init()
{
	m_SP = new CPU::Stack<int16_t>(reinterpret_cast<int16_t*>(&m_RAM->front()), 16);
	InitFonts();
	printf_s("Chip8 initialized! Memory capacity: %uB\n", m_RAM->size());
}

void Chip8::Reset()
{
	m_V.fill(0);
	m_I = NULLPTR;
	m_PC = USER_SPACE_ADDR;
	m_ProgramEnd = USER_SPACE_ADDR;
	m_Delay = m_Sound = 0;
}

void Chip8::LoadProgram(const Memory & mem)
{
	if (mem.size() + USER_SPACE_ADDR > m_RAM->size())
		throw "Memory insufficient!";
	Reset();
	std::copy(mem.begin(), mem.end(), m_RAM->begin() + USER_SPACE_ADDR);
	m_ProgramEnd = USER_SPACE_ADDR + mem.size();
}

bool Chip8::ExecuteInstruction()
{
	Opcode opcode;
	if(!ReadInsruction(opcode))
		return false;

	//printf_s("Instruction received: 0x%X\n", opcode);
	
	InstructionMap::const_iterator it;
	for (size_t i = 0; i < m_OpcodeMasks.size(); i++)
	{
		// Check if the opcode masked the current mask is in the map of valid instructions 
		it = m_Instructions.find(opcode & m_OpcodeMasks.at(i));
		if (it != m_Instructions.end()) {
			(this->*(it->second))(opcode); // Call the instruction handler
			return true;
		}
	}

	printf_s("Unknown instruction 0x%X! Skipping...\n", opcode);
	return true;
}

void Chip8::Instruction0NNN(const Opcode& opcode)
{
	printf_s("Got an unimplented SYS instruction (Opcode: 0x%X). Skipping...\n", opcode);
}

void Chip8::Instruction00E0(const Opcode& opcode)
{
	if (!m_Peripherals->ClearScreen())
		printf_s("Failed to clear the screen! %s\n", SDL_GetError());
}

void Chip8::Instruction00EE(const Opcode& opcode)
{
	m_PC = static_cast<Address>(m_SP->pop());
}

void Chip8::Instruction1NNN(const Opcode& opcode)
{
	m_PC = ExtractAddress(opcode);
}

void Chip8::Instruction2NNN(const Opcode& opcode)
{
	m_SP->push(m_PC);
	m_PC = ExtractAddress(opcode);
}

void Chip8::Instruction3XNN(const Opcode& opcode)
{
	if (GetRegisterValue(opcode, false) == ExtractByte(opcode))
		m_PC += INSTRUCTION_SIZE; // Skip 1 instruction
}

void Chip8::Instruction4XNN(const Opcode& opcode)
{
	if (GetRegisterValue(opcode, false) != ExtractByte(opcode))
		m_PC += INSTRUCTION_SIZE; // Skip 1 instruction
}

void Chip8::Instruction5XY0(const Opcode& opcode)
{
	if (GetRegisterValue(opcode, false) == GetRegisterValue(opcode, true))
		m_PC += INSTRUCTION_SIZE; // Skip 1 instruction
}

void Chip8::Instruction6XNN(const Opcode& opcode)
{
	m_V.at(ExtractRegisterId(opcode, false)) = ExtractByte(opcode);
}

void Chip8::Instruction7XNN(const Opcode& opcode)
{
	m_V.at(ExtractRegisterId(opcode, false)) += ExtractByte(opcode);
}

void Chip8::Instruction8XY0(const Opcode& opcode)
{
	m_V.at(ExtractRegisterId(opcode, false)) = GetRegisterValue(opcode, true);
}

void Chip8::Instruction8XY1(const Opcode& opcode)
{
	m_V.at(ExtractRegisterId(opcode, false)) |= GetRegisterValue(opcode, true);
}

void Chip8::Instruction8XY2(const Opcode& opcode)
{
	m_V.at(ExtractRegisterId(opcode, false)) &= GetRegisterValue(opcode, true);
}

void Chip8::Instruction8XY3(const Opcode& opcode)
{
	m_V.at(ExtractRegisterId(opcode, false)) ^= GetRegisterValue(opcode, true);
}

void Chip8::Instruction8XY4(const Opcode& opcode)
{
	size_t lhs = ExtractRegisterId(opcode, false);
	int result = static_cast<int>(m_V.at(lhs)) + static_cast<int>(GetRegisterValue(opcode, true));

	// Store the result of the addition
	m_V.at(lhs) = static_cast<Byte>(result & 0xFF);
	// Store the carry
	m_V.at(0xF) = static_cast<Byte>(result & 0xF00);
}

void Chip8::Instruction8XY5(const Opcode& opcode)
{
	size_t lhs = ExtractRegisterId(opcode, false);
	int result = static_cast<int>(m_V.at(lhs)) - static_cast<int>(GetRegisterValue(opcode, true));

	// Store the result of the subtraction
	m_V.at(lhs) = static_cast<Byte>(result & 0xFF);
	// Store the borrow (0 on borrow)
	m_V.at(0xF) = (result < 0) ? 0 : 1;
}

void Chip8::Instruction8XY6(const Opcode& opcode)
{
	size_t lhs = ExtractRegisterId(opcode, false);
	// Store the LSB in V[F]
	m_V.at(0xF) = m_V.at(lhs) & 1;
	// Right shift the register by 1 bit
	m_V.at(lhs) >>= 1;
}

void Chip8::Instruction8XY7(const Opcode& opcode)
{
	size_t lhs = ExtractRegisterId(opcode, false);
	int result = static_cast<int>(GetRegisterValue(opcode, true)) - static_cast<int>(m_V.at(lhs));

	// Store the result of the subtraction
	m_V.at(lhs) = static_cast<Byte>(result & 0xFF);
	// Store the borrow (0 on borrow)
	m_V.at(0xF) = (result < 0) ? 0 : 1;
}

void Chip8::Instruction8XYE(const Opcode& opcode)
{
	size_t lhs = ExtractRegisterId(opcode, false);
	// Store the MSB in V[F]
	m_V.at(0xF) = m_V.at(lhs) & 0x80;
	// Left shift the register by 1 bit
	m_V.at(lhs) <<= 1;
}

void Chip8::Instruction9XY0(const Opcode& opcode)
{
	if (GetRegisterValue(opcode, false) != GetRegisterValue(opcode, true))
		m_PC += INSTRUCTION_SIZE; // Skip 1 instruction
}

void Chip8::InstructionANNN(const Opcode& opcode)
{
	m_I = ExtractAddress(opcode);
}

void Chip8::InstructionBNNN(const Opcode& opcode)
{
	m_PC = ExtractAddress(opcode) + static_cast<Address>(m_V.at(0));
}

void Chip8::InstructionCXNN(const Opcode& opcode)
{
	m_V.at(ExtractRegisterId(opcode, false)) = GenerateByte() & ExtractByte(opcode);
}

void Chip8::InstructionDXYN(const Opcode& opcode)
{
	uint8_t x = GetRegisterValue(opcode, false);
	uint8_t y = GetRegisterValue(opcode, true);
	uint8_t w = 8;
	uint8_t h = ExtractNibble(opcode);

	SDL_Surface* surface = m_Peripherals->GetSurface(1);
	uint32_t* pixels = reinterpret_cast<uint32_t*>(surface->pixels);
	uint32_t xor_val = -1; // Intentional underflow

	SDL_LockSurface(surface);
	bool collision = false;
	for (size_t i = 0; i < h; i++)
	{
		Byte sprite = m_RAM->at(m_I + i);
		for (size_t j = 0; j < w; j++)
		{
			Byte b = sprite >> (w - j - 1) & 0x1;
			size_t index = ((i + y) * surface->w + j + x);
			if (!collision && (pixels[index] & xor_val))
				collision = true;
			pixels[index] ^= b ? xor_val : 0;
		}
	}
	SDL_UnlockSurface(surface);
	m_V[0xF] = collision ? 1 : 0;
	/*
	SDL_Rect r = { GetRegisterValue(opcode, false), GetRegisterValue(opcode, true), 8, ExtractNibble(opcode) };
	// ToDo(Ivan): This is absolutely wrong! Fix it!
	if (!m_Peripherals->FillRect(&r, 255, 255, 255))
		printf_s("Failed to draw rect! %s", SDL_GetError());
	*/
}

void Chip8::InstructionEX9E(const Opcode& opcode)
{
	if(m_Peripherals->IsKeyPressed(GetRegisterValue(opcode, false)))
		m_PC += INSTRUCTION_SIZE; // Skip 1 instruction
}

void Chip8::InstructionEXA1(const Opcode& opcode)
{
	if (!m_Peripherals->IsKeyPressed(GetRegisterValue(opcode, false)))
		m_PC += INSTRUCTION_SIZE; // Skip 1 instruction
}

void Chip8::InstructionFX07(const Opcode& opcode)
{
	m_V.at(ExtractRegisterId(opcode, false)) = m_Delay;
}

void Chip8::InstructionFX0A(const Opcode& opcode)
{
	while (!m_Peripherals->IsKeyPressed(GetRegisterValue(opcode, false)))
		SDL_Delay(16); // Wait 16ms(1 frame) and try again
}

void Chip8::InstructionFX15(const Opcode& opcode)
{
	m_Delay = GetRegisterValue(opcode, false);
}

void Chip8::InstructionFX18(const Opcode& opcode)
{
	m_Sound = GetRegisterValue(opcode, false);
}

void Chip8::InstructionFX1E(const Opcode& opcode)
{
	m_I += GetRegisterValue(opcode, false);
}

void Chip8::InstructionFX29(const Opcode& opcode)
{
	// This is probably not going to work if there are custom sprites stored
	// (if this is even possible)
	Address memoryOffset = GetRegisterValue(opcode, false) * 5;
	/* ToDo(Ivan): probably can add bounds check here */
	m_I = memoryOffset;
}

void Chip8::InstructionFX33(const Opcode& opcode)
{
	Byte value = GetRegisterValue(opcode, false);
	m_RAM->at(m_I) = value / 100;
	m_RAM->at(m_I + 1) = value / 10 % 10;
	m_RAM->at(m_I + 2) = value % 10;
}

void Chip8::InstructionFX55(const Opcode& opcode)
{
	size_t endRegister = ExtractRegisterId(opcode, false);
	
	for (size_t i = 0; i < endRegister; i++)
		m_RAM->at(m_I + i) = m_V.at(i);
}

void Chip8::InstructionFX65(const Opcode& opcode)
{
	size_t endRegister = ExtractRegisterId(opcode, false);

	for (size_t i = 0; i < endRegister; i++)
		m_V.at(i) = m_RAM->at(m_I + i);
}

inline Chip8::Address Chip8::ExtractAddress(const Opcode& opcode)
{
	return static_cast<Address>(opcode & 0xFFF);
}

inline Byte Chip8::ExtractByte(const Opcode& opcode)
{
	return static_cast<Byte>(opcode & 0xFF);
}

inline Byte Chip8::ExtractNibble(const Opcode& opcode)
{
	return static_cast<Byte>(opcode & 0xF);
}

inline size_t Chip8::ExtractRegisterId(const Opcode& opcode, const bool rhs)
{
	return (opcode >> (rhs ? 4 : 8)) & 0xF;
}

inline Chip8::Register8 Chip8::GetRegisterValue(const Opcode& opcode, const bool rhs)
{
	return m_V.at(ExtractRegisterId(opcode, rhs));
}

inline Byte Chip8::GenerateByte()
{
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, 255);
	return static_cast<Byte>(dist(m_RNG));
}

inline void Chip8::InitFonts()
{
	Memory m = {
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

	std::copy(m.begin(), m.end(), m_RAM->begin());
}

inline bool Chip8::ReadInsruction(Opcode & opcode)
{
	if(m_PC > m_ProgramEnd)
		return false;
	opcode = m_RAM->at(m_PC) << 8 | m_RAM->at(m_PC + 1);
	m_PC += INSTRUCTION_SIZE; // Move to the next instruction
	return true;
}
