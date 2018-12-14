#include "VM.h"

VM::VM(CPU * processor, const size_t & RAMSize)
	:m_CPU(processor), m_RAM(RAMSize)
{
	m_Peripherals.Init();
	m_CPU->UseMemory(&m_RAM);
	m_CPU->UsePeripherals(&m_Peripherals);
	m_CPU->Init();
}

VM::~VM()
{
}

void VM::MapKeyCodes(const SDLAPI::KeyMap & keymap)
{
	m_Peripherals.SetKeyMap(keymap);
}

void VM::Start(const char* filename)
{
	{
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (!file.is_open()) {
			return;
		}
		size_t size = file.tellg();
		file.seekg(0, file.beg);
		Memory m;
		m.reserve(size);
		std::copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), std::back_inserter(m));
		file.close();
		m_CPU->LoadProgram(m);
	}

	if (!m_Peripherals.CreateWindow("Chip8 test", 256, 128)) {
		printf_s("Could not create window! %s\n", SDL_GetError());
		exit(1);
	}

	m_Peripherals.RunGameLoop(
		nullptr, 
		[&](SDLAPI* handle) { 
			if(!m_CPU->ExecuteInstruction())
				handle->Quit(); 
		},
		[&](SDLAPI* handle) {
			auto s = handle->GetSurface();
			auto s1 = handle->GetSurface(1);
			
			SDL_Rect display = { 0, 0, 64, 32 };
			SDL_Rect stretch = { 0, 0, s->w, s->h };
			handle->UpdateWindow();
			SDL_BlitScaled(s1, &display, s, &stretch);
			handle->UpdateWindow();
		},
		[&](const SDLAPI::ErrorType& type, const char* msg) {
			std::string typeStr;
			switch (type)
			{
			case SDLAPI::ErrorType::NOTICE:
				typeStr = "NOTICE";
				break;
			case SDLAPI::ErrorType::WARNING:
				typeStr = "WARNING";
				break;
			case SDLAPI::ErrorType::ERROR:
				typeStr = "ERROR";
				break;
			case SDLAPI::ErrorType::CRITICAL:
				typeStr = "CRITICAL";
				break;
			default:
				typeStr = "UNKNOWN";
				break;
			}
			printf_s("(%s): %s %s\n", typeStr.c_str(), msg, SDL_GetError());
		}
	);
}
