#pragma once
#include <SDL.h>
#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <atomic>




class SDLAPI
{
public:
	typedef const Uint64 Cui64;
	typedef const Uint32 Cui32;
	typedef const Uint16 Cui16;
	typedef const Uint8 Cui8;
	typedef std::unordered_map<Uint16, SDL_Scancode> KeyMap;
	enum class ErrorType { NOTICE, WARNING, ERROR, CRITICAL };

	typedef std::function<void(SDLAPI*)> Function;
	typedef std::function<void(const ErrorType&, const char*)> ErrorHandler;

	SDLAPI();
	SDLAPI(Cui32& initFlags);
	~SDLAPI();
	bool Init();
	void SetInitFlags(Cui32& flags);
	void SetKeyMap(const KeyMap& keymap);
	void SetWindowFlags(Cui32& flags);
	SDL_Window* GetWindow() const;
	SDL_Surface* GetSurface(const size_t& index = 0) const;
	bool CreateWindow(const std::string& title, Cui32& width, Cui32& height, Cui32& posX = SDL_WINDOWPOS_UNDEFINED, Cui32& posY = SDL_WINDOWPOS_UNDEFINED);
	void RunGameLoop(Function events = nullptr, Function update = nullptr, Function render = nullptr, ErrorHandler eh = nullptr);
	void Quit();
	bool UpdateWindow();
	bool FillRect(const SDL_Rect* rect, Cui32& color);
	bool FillRect(const SDL_Rect* rect, Cui8& r, Cui8& g, Cui8& b);
	bool FillRect(const SDL_Rect* rect, Cui8& r, Cui8& g, Cui8& b, Cui8& a);
	bool ClearScreen();
	bool SetDrawColor(Cui8& r, Cui8& g, Cui8& b, Cui8& a);

	bool IsKeyPressed(const KeyMap::key_type& key);

	Uint32 RGB(Cui8& r, Cui8& g, Cui8& b);
	Uint32 RGBA(Cui8& r, Cui8& g, Cui8& b, Cui8& a);
private:
	SDL_Window* m_Window = nullptr;
	SDL_Surface* m_Surface[2] = { nullptr };
	SDL_Renderer* m_Renderer = nullptr;
	SDL_Color m_DrawColor = { 0, 0, 0, 0xFF };
	Uint32 m_InitFlags = SDL_INIT_VIDEO;
	Uint32 m_WindowFlags = SDL_WINDOW_SHOWN;
	std::atomic<bool> m_Running = false;
	KeyMap m_Keymap;
};
