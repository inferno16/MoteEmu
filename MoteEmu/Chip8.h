#pragma once
/* Specification info taken from: https://en.wikipedia.org/wiki/CHIP-8 */

#include <array>
#include <unordered_map>
#include <memory>
#include <random>

#include "CPU.h"

class Chip8 : public CPU
{
public:
	typedef Byte Register8;
	typedef uint16_t Address;
	typedef uint16_t OpcodeMask;
	typedef Address Register16;
	typedef Byte Timer;
	typedef std::unordered_map<OpcodeMask, void (Chip8::*)(const Opcode&)> InstructionMap;

	static constexpr const Address NULLPTR = 0;
	static constexpr const Byte INSTRUCTION_SIZE = 2;
	static constexpr const Address USER_SPACE_ADDR = 0x200;
	

	Chip8();
	~Chip8();

	void Init() override;
	void Reset() override;
	void LoadProgram(const Memory& mem) override;
	bool ExecuteInstruction() override;

private:
	/* 
	CHIP-8 has 35 opcodes, which are all two bytes long and stored big-endian.
	The opcodes are listed below, in hexadecimal and with the following symbols:
		- NNN: address
		- NN: 8-bit constant
		- N: 4-bit constant
		- X and Y: 4-bit register identifier
		- PC : Program Counter
		- I : 16bit register (For memory address) (Similar to void pointer)
	*/

	// Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
	void Instruction0NNN(const Opcode& opcode);

	// Clears the screen.
	void Instruction00E0(const Opcode& opcode);

	// Returns from a subroutine.
	void Instruction00EE(const Opcode& opcode);

	// Jumps to address NNN.
	void Instruction1NNN(const Opcode& opcode);

	// Calls subroutine at NNN.
	void Instruction2NNN(const Opcode& opcode);

	// Skips the next instruction if VX equals NN.
	// (Usually the next instruction is a jump to skip a code block)
	void Instruction3XNN(const Opcode& opcode);

	// Skips the next instruction if VX doesn't equal NN.
	// (Usually the next instruction is a jump to skip a code block)
	void Instruction4XNN(const Opcode& opcode);

	// Skips the next instruction if VX equals VY.
	// (Usually the next instruction is a jump to skip a code block)
	void Instruction5XY0(const Opcode& opcode);

	// Sets VX to NN.
	void Instruction6XNN(const Opcode& opcode);

	// Adds NN to VX. (Carry flag is not changed)
	void Instruction7XNN(const Opcode& opcode);

	// Sets VX to the value of VY.
	void Instruction8XY0(const Opcode& opcode);

	// Sets VX to VX or VY. (Bitwise OR operation)
	void Instruction8XY1(const Opcode& opcode);

	// Sets VX to VX and VY. (Bitwise AND operation)
	void Instruction8XY2(const Opcode& opcode);

	// Sets VX to VX xor VY.
	void Instruction8XY3(const Opcode& opcode);

	// Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
	void Instruction8XY4(const Opcode& opcode);

	// VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
	void Instruction8XY5(const Opcode& opcode);

	// Stores the least significant bit of VX in VF and then shifts VX to the right by 1.
	void Instruction8XY6(const Opcode& opcode);

	// Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
	void Instruction8XY7(const Opcode& opcode);

	// Stores the most significant bit of VX in VF and then shifts VX to the left by 1.
	void Instruction8XYE(const Opcode& opcode);

	// Skips the next instruction if VX doesn't equal VY.
	// (Usually the next instruction is a jump to skip a code block)
	void Instruction9XY0(const Opcode& opcode);

	// Sets I to the address NNN.
	void InstructionANNN(const Opcode& opcode);

	// Jumps to the address NNN plus V0.
	void InstructionBNNN(const Opcode& opcode);

	// Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
	void InstructionCXNN(const Opcode& opcode);

	/* Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels
	and a height of N pixels. Each row of 8 pixels is read as bit-coded
	starting from memory location I; I value doesn’t change after the
	execution of this instruction. As described above, VF is set to 1
	if any screen pixels are flipped from set to unset when the sprite
	is drawn, and to 0 if that doesn’t happen*/
	void InstructionDXYN(const Opcode& opcode);

	// Skips the next instruction if the key stored in VX is pressed.
	// (Usually the next instruction is a jump to skip a code block)
	void InstructionEX9E(const Opcode& opcode);

	// Skips the next instruction if the key stored in VX isn't pressed.
	// (Usually the next instruction is a jump to skip a code block)
	void InstructionEXA1(const Opcode& opcode);

	// Sets VX to the value of the delay timer.
	void InstructionFX07(const Opcode& opcode);

	// A key press is awaited, and then stored in VX.
	// (Blocking Operation. All instruction halted until next key event)
	void InstructionFX0A(const Opcode& opcode);

	// Sets the delay timer to VX.
	void InstructionFX15(const Opcode& opcode);

	// Sets the sound timer to VX.
	void InstructionFX18(const Opcode& opcode);

	// Adds VX to I.
	void InstructionFX1E(const Opcode& opcode);

	// Sets I to the location of the sprite for the character in VX. 
	// Characters 0-F (in hexadecimal) are represented by a 4x5 font.
	void InstructionFX29(const Opcode& opcode);

	/* Stores the binary-coded decimal representation of VX, with the
	most significant of three digits at the address in I, the middle
	digit at I plus 1, and the least significant digit at I plus 2.
	(In other words, take the decimal representation of VX, place the
	hundreds digit in memory at location in I, the tens digit at location
	I+1, and the ones digit at location I+2.)*/
	void InstructionFX33(const Opcode& opcode);

	// Stores V0 to VX (including VX) in memory starting at address I. 
	// The offset from I is increased by 1 for each value written, but I itself is left unmodified.
	void InstructionFX55(const Opcode& opcode);

	// Fills V0 to VX (including VX) with values from memory starting at address I.
	// The offset from I is increased by 1 for each value written, but I itself is left unmodified.
	void InstructionFX65(const Opcode& opcode);

	// Helper methods for the instructions

	inline Address ExtractAddress(const Opcode& opcode);
	inline Byte ExtractByte(const Opcode& opcode);
	inline Byte ExtractNibble(const Opcode& opcode);
	inline size_t ExtractRegisterId(const Opcode& opcode, const bool rhs);
	inline Register8 GetRegisterValue(const Opcode& opcode, const bool rhs);
	inline Byte GenerateByte();

	// Other methods

	inline void InitFonts();
	inline bool ReadInsruction(Opcode& opcode);

private:
	std::array<Register8, 16> m_V = {0};
	Register16 m_I = 0;
	Address m_PC = USER_SPACE_ADDR;
	Address m_ProgramEnd = USER_SPACE_ADDR;
	CPU::Stack<int16_t>* m_SP = nullptr;
	Timer m_Delay = 0;
	Timer m_Sound = 0;
	std::mt19937 m_RNG;
	const std::array<OpcodeMask, 4> m_OpcodeMasks = { 0xFFFF, 0xF000, 0xF00F, 0xF0FF };
	const InstructionMap m_Instructions = {
		{ 0x0000, &Chip8::Instruction0NNN },
		{ 0x00E0, &Chip8::Instruction00E0 },
		{ 0x00EE, &Chip8::Instruction00EE },
		{ 0x1000, &Chip8::Instruction1NNN },
		{ 0x2000, &Chip8::Instruction2NNN },
		{ 0x3000, &Chip8::Instruction3XNN },
		{ 0x4000, &Chip8::Instruction4XNN },
		{ 0x5000, &Chip8::Instruction5XY0 },
		{ 0x6000, &Chip8::Instruction6XNN },
		{ 0x7000, &Chip8::Instruction7XNN },
		{ 0x8000, &Chip8::Instruction8XY0 },
		{ 0x8001, &Chip8::Instruction8XY1 },
		{ 0x8002, &Chip8::Instruction8XY2 },
		{ 0x8003, &Chip8::Instruction8XY3 },
		{ 0x8004, &Chip8::Instruction8XY4 },
		{ 0x8005, &Chip8::Instruction8XY5 },
		{ 0x8006, &Chip8::Instruction8XY6 },
		{ 0x8007, &Chip8::Instruction8XY7 },
		{ 0x800E, &Chip8::Instruction8XYE },
		{ 0x9000, &Chip8::Instruction9XY0 },
		{ 0xA000, &Chip8::InstructionANNN },
		{ 0xB000, &Chip8::InstructionBNNN },
		{ 0xC000, &Chip8::InstructionCXNN },
		{ 0xD000, &Chip8::InstructionDXYN },
		{ 0xE09E, &Chip8::InstructionEX9E },
		{ 0xE0A1, &Chip8::InstructionEXA1 },
		{ 0xF007, &Chip8::InstructionFX07 },
		{ 0xF00A, &Chip8::InstructionFX0A },
		{ 0xF015, &Chip8::InstructionFX15 },
		{ 0xF018, &Chip8::InstructionFX18 },
		{ 0xF01E, &Chip8::InstructionFX1E },
		{ 0xF029, &Chip8::InstructionFX29 },
		{ 0xF033, &Chip8::InstructionFX33 },
		{ 0xF055, &Chip8::InstructionFX55 },
		{ 0xF065, &Chip8::InstructionFX65 }
	};
};
