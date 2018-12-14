#include "SDLAPI.h"

SDLAPI::SDLAPI()
{
}

SDLAPI::SDLAPI(Cui32& initFlags)
	:m_InitFlags(initFlags)
{
	Init();
}

SDLAPI::~SDLAPI()
{
	SDL_FreeSurface(m_Surface[0]);
	SDL_FreeSurface(m_Surface[1]);
	SDL_DestroyRenderer(m_Renderer);
	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}

bool SDLAPI::Init()
{
	return SDL_Init(m_InitFlags) < 0;
}

void SDLAPI::SetInitFlags(Cui32& flags)
{
	m_InitFlags = flags;
}

void SDLAPI::SetKeyMap(const KeyMap & keymap)
{
}

void SDLAPI::SetWindowFlags(Cui32& flags)
{
	m_WindowFlags = flags;
}

SDL_Window* SDLAPI::GetWindow() const
{
	return m_Window;
}

SDL_Surface* SDLAPI::GetSurface(const size_t& index) const
{
	return m_Surface[index];
}

bool SDLAPI::CreateWindow(const std::string& title, Cui32& width, Cui32& height, Cui32& posX, Cui32& posY)
{
	m_Window = SDL_CreateWindow(title.c_str(), posX, posY, width, height, m_WindowFlags);
	bool windowStatus = m_Window != nullptr;
	if (windowStatus) {
		auto s = SDL_GetWindowSurface(m_Window);
		m_Surface[0] = s;
		auto f = s->format;
		char* pixels = new char[s->h * s->pitch];
		m_Surface[1] = SDL_CreateRGBSurfaceWithFormatFrom(pixels, s->w, s->h, f->BitsPerPixel / f->BytesPerPixel, s->pitch, f->format);
		m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);
		windowStatus = SDL_SetRenderDrawColor(m_Renderer, m_DrawColor.r, m_DrawColor.b, m_DrawColor.g, m_DrawColor.a) == 0;
	}
	return windowStatus && m_Surface[0] != nullptr && m_Surface[1] != nullptr && m_Renderer != nullptr;
}

void SDLAPI::RunGameLoop(Function events, Function update, Function render, ErrorHandler error)
{
	if (error == nullptr)
		error = [](const ErrorType&, const char*) {};
	if (m_Window == nullptr) {
		error(ErrorType::CRITICAL, "Window is not created!");
		return;
	}
	if (m_Surface == nullptr) {
		error(ErrorType::CRITICAL, "Surface is not available!");
		return;
	}

	m_Running = true;

	while (m_Running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EventType::SDL_QUIT) {
				m_Running = false;
				break;
			}
			if (events != nullptr)
				events(this);
		}
		if (update != nullptr)
			update(this);
		if (render != nullptr)
			render(this);
		if (!UpdateWindow()) {
			error(ErrorType::WARNING, "Failed to update window!");
		}
	}
}

void SDLAPI::Quit()
{
	m_Running = false;
}

bool SDLAPI::UpdateWindow()
{
	return SDL_UpdateWindowSurface(m_Window) == 0;
}

bool SDLAPI::FillRect(const SDL_Rect * rect, Cui32 & color)
{
	return SDL_FillRect(m_Surface[0], rect, color) == 0;
}

bool SDLAPI::FillRect(const SDL_Rect * rect, Cui8 & r, Cui8 & g, Cui8 & b)
{
	return FillRect(rect, RGB(r,g,b));
}

bool SDLAPI::FillRect(const SDL_Rect * rect, Cui8 & r, Cui8 & g, Cui8 & b, Cui8 & a)
{
	return FillRect(rect, RGBA(r, g, b, a));
}

bool SDLAPI::ClearScreen()
{
	bool status = SDL_RenderClear(m_Renderer) == 0;
	if (status) {
		size_t size = m_Surface[0]->h * m_Surface[0]->pitch;
		memcpy_s(m_Surface[1]->pixels, size, m_Surface[0]->pixels, size);
	}
	return status;
}

bool SDLAPI::SetDrawColor(Cui8 & r, Cui8 & g, Cui8 & b, Cui8 & a)
{
	m_DrawColor = { r, g, b, a };
	if(m_Renderer != nullptr)
		return SDL_SetRenderDrawColor(m_Renderer, r, g, b, a) == 0;
	return true;
}

bool SDLAPI::IsKeyPressed(const KeyMap::key_type& key)
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	return state[((m_Keymap.find(key) != m_Keymap.end()) ? m_Keymap.at(key) : key)];
}

Uint32 SDLAPI::RGB(Cui8 & r, Cui8 & g, Cui8 & b)
{
	return SDL_MapRGB(m_Surface[0]->format, r, g, b);
}

Uint32 SDLAPI::RGBA(Cui8 & r, Cui8 & g, Cui8 & b, Cui8 & a)
{
	return SDL_MapRGBA(m_Surface[0]->format, r, g, b, a);
}
