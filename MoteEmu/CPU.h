#pragma once

#include <vector>
#include <stdint.h>

#include "SDLAPI.h"

typedef unsigned char Byte;
typedef std::vector<Byte> Memory;

class CPU
{
public:
	template<typename T>
	class Stack {
	public: 
		Stack(T* baseAddr, const size_t& size);
		void push(const T& val);
		T pop();
	private:
		T* m_SP = nullptr;
		T* m_SBP = nullptr;
		size_t m_Size;
	};
	typedef uint32_t Opcode;

	explicit CPU();
	explicit CPU(Memory* RAM, SDLAPI* peripherals);
	virtual ~CPU();

	void UseMemory(Memory* RAM);
	void UsePeripherals(SDLAPI* peripherals);
	
	virtual void Init() = 0;
	virtual void Reset() = 0;
	virtual void LoadProgram(const Memory&) = 0;
	virtual bool ExecuteInstruction() = 0;

protected:
	Memory* m_RAM;
	SDLAPI* m_Peripherals;
};

template<typename T>
inline CPU::Stack<T>::Stack(T* baseAddr, const size_t& size)
	: m_SBP(baseAddr), m_SP(baseAddr), m_Size(size)
{}

template<typename T>
inline void CPU::Stack<T>::push(const T & val)
{
	if (std::abs(m_SBP - m_SP) > m_Size)
		throw std::overflow_error("Stack overflow!");
	*(m_SP++) = val;
}

template<typename T>
inline T CPU::Stack<T>::pop()
{
	if (m_SBP - m_SP == 0)
		throw std::underflow_error("Stack underflow!");
	return *(--m_SP);
}
