#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#if __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_ttf.h>
#endif
#include "preproc.h"
#include "gamemanager.h"
#include "gamestate.h"
#include <string>
#if SOUND_ENABLED
#include <fmod.hpp>
#include <fmod_errors.h>
#endif

class CGameState;

// Main App Singleton Class
class CApp 
{

private:
	static CApp* instance;

public:
	CApp() {}
	CApp(const CApp& appObj) = delete; // copy constructor

	static CApp* GetInstance(bool createInstance = true) // singleton's getinstance function
	{
		if (instance == nullptr && createInstance)
		{
			instance = new CApp();
			return instance;
		}
		
		return instance;
	}

public:

	void Init();
	void CleanUp();

	//********** GRAPHICS *********************************************************
	
	void InitSDL();
	void CleanUpSDL();

	void LoadFonts();
	void CleanUpFonts();

	void DrawText(int x, int y, const char* text, SDL_Color color);

	const int GetScreenWidth() { return g_screenWidth; }
	const int GetScreenHeight() { return g_screenHeight; }

	SDL_Renderer* GetRenderer() { return m_renderer; }
	TTF_Font* GetRegularFont() { return m_regularFont; }
	TTF_Font* GetBigFont() { return m_bigFont; }

#if SOUND_ENABLED
	FMOD::System* GetFmodSystem() { return m_fmodSystem; }
	FMOD::ChannelGroup* GetFmodChannelGroup() { return m_channelGroup; }
#endif
	
	void DrawLine(int x1, int y1, int x2, int y2, SDL_Color color);
	void DrawBox(int x, int y, int w, int h, bool fill, SDL_Color foregroundColor, SDL_Color backgroundColor);
	
	//********** SOUND *********************************************************

#if SOUND_ENABLED
	void InitFMOD();
	void CleanUpFMOD();
	bool PrepareSoundChannel(FMOD::Channel* channel);
	void SetChannelVolume(FMOD::Channel* channel, float volume);
#endif

	//********** APP *********************************************************

	CGameManager* GetGameManager() { return &m_gameManager; }

	void HandleInput();

	void PrepareScene();
	void PresentScene();

	void Update();	

private:
	const std::string FONT_FACE_FILENAME = "retrogaming.ttf";
	const int REGULAR_FONT_SIZE_PT = 24;
	const int BIG_FONT_SIZE_PT = 72;

	// SDL objects
	SDL_Renderer* m_renderer = nullptr;
	SDL_Window* m_window = nullptr;

	// Font
	TTF_Font* m_regularFont = nullptr;
	TTF_Font* m_bigFont = nullptr;

#if SOUND_ENABLED
	// FMOD objects
	FMOD::System* m_fmodSystem = nullptr;
	FMOD::ChannelGroup* m_channelGroup = nullptr;
#endif

	CGameManager m_gameManager;

	// app variables / objects
	const int g_screenWidth = GFX_SCREEN_WIDTH;
	const int g_screenHeight = GFX_SCREEN_HEIGHT;
};

void UpdateApp();
