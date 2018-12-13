#include <memory>
#include <stdio.h>

#include "VM.h"
#include "Chip8.h"
#include "test.h"

#ifndef TEST
int main(int argc, char** argv) {
#else
int _main(int argc, char** argv) {
#endif // !TEST

	if (argc != 2) {
		printf_s("%u arguments passed. Expected: %u.\n", argc, 2);
		return 1;
	}
	Chip8 chip8;
	VM vm(&chip8, 1024 * 4);
	vm.MapKeyCodes({
		{ 0x0, SDL_Scancode::SDL_SCANCODE_X },
		{ 0x1, SDL_Scancode::SDL_SCANCODE_1 },
		{ 0x2, SDL_Scancode::SDL_SCANCODE_2 },
		{ 0x3, SDL_Scancode::SDL_SCANCODE_3 },
		{ 0x4, SDL_Scancode::SDL_SCANCODE_Q },
		{ 0x5, SDL_Scancode::SDL_SCANCODE_W },
		{ 0x6, SDL_Scancode::SDL_SCANCODE_E },
		{ 0x7, SDL_Scancode::SDL_SCANCODE_A },
		{ 0x8, SDL_Scancode::SDL_SCANCODE_S },
		{ 0x9, SDL_Scancode::SDL_SCANCODE_D },
		{ 0xA, SDL_Scancode::SDL_SCANCODE_Z },
		{ 0xB, SDL_Scancode::SDL_SCANCODE_C },
		{ 0xC, SDL_Scancode::SDL_SCANCODE_4 },
		{ 0xD, SDL_Scancode::SDL_SCANCODE_R },
		{ 0xE, SDL_Scancode::SDL_SCANCODE_F },
		{ 0xF, SDL_Scancode::SDL_SCANCODE_V },
	});
	vm.Start(argv[1]);
	return 0;
}
