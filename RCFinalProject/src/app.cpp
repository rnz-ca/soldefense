/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include <stdio.h>
#include <string>
#include <assert.h>
#if __APPLE__
#include <SDL2_image/SDL_image.h>
#else
#include <SDL_Image.h>
#endif
#include "app.h"
#include <time.h>
#include "utils.h"

// app singleton
CApp* CApp::instance = nullptr;

//********** INITIALIZATION / CLEANUP *********************************************************
void CleanUpApp()
{
	// destroy app singleton
	CApp* app = CApp::GetInstance(false); // get a poiner to the singleton but if it does not exist, do not create it
	if (app != nullptr)
	{
		app->CleanUp();
	}
	delete app;
}

void UpdateApp()
{		
	atexit(CleanUpApp);
	CApp* app = CApp::GetInstance(); // create the singleton
	app->Init();
	app->Update();
}

//********** APP CONSTRUCTOR / DESTRUCTOR *********************************************************

void CApp::Init()
{
	// randomize rng seed
	Utils::RandomizeRngSeed();	

	InitSDL();
	LoadFonts();
#if SOUND_ENABLED
	InitFMOD();
#endif
}

void CApp::CleanUp()
{
	m_gameManager.Destroy();
#if SOUND_ENABLED
	CleanUpFMOD();
#endif
	CleanUpFonts();
	CleanUpSDL();
}

//********** SDL INITIALIZATION / CLEANUP *********************************************************
void CApp::InitSDL()
{
	LOG_SCR("Initializing SDL");
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG_SCR_F("Unable to initialize the SDL Library: %s\n", SDL_GetError());
		exit(1);
	}

	m_window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, g_screenWidth, g_screenHeight, 0);
	if (m_window == nullptr)
	{
		LOG_SCR_F("Unable to open %dx%d window: %s\n", g_screenWidth, g_screenHeight, SDL_GetError());
		exit(1);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
	if (m_renderer == nullptr)
	{
		LOG_SCR_F("Unable to create renderer: %s\n", SDL_GetError());
		exit(1);
	}

	// initialize the SDL_Image library
	IMG_Init(IMG_INIT_PNG);

	// initialize the SDL_TTF library
	if (TTF_Init() == -1)
	{
		LOG_SCR_F("Unable to initialize the SDL Font Library: %s\n", TTF_GetError());
		exit(1);
	}
}

void CApp::CleanUpSDL()
{
	LOG_SCR("Cleaning up SDL");
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void CApp::DrawLine(int x1, int y1, int x2, int y2, SDL_Color color)
{
	SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawLine(m_renderer, x1, y1, x2, y2);
}

void CApp::DrawBox(int x, int y, int w, int h, bool fill, SDL_Color foregroundColor, SDL_Color backgroundColor)
{
	SDL_Rect rc{ x, y, w, h };
	SDL_BlendMode originalBlendMode;
	SDL_GetRenderDrawBlendMode(m_renderer, &originalBlendMode);

	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	if (fill)
	{		
		SDL_SetRenderDrawColor(m_renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
		SDL_RenderFillRect(m_renderer, &rc);
	}
	SDL_SetRenderDrawColor(m_renderer, foregroundColor.r, foregroundColor.g, foregroundColor.b, foregroundColor.a);
	SDL_RenderDrawRect(m_renderer, &rc);
	SDL_SetRenderDrawBlendMode(m_renderer, originalBlendMode);
}

void CApp::LoadFonts()
{
	LOG_SCR("Loading Fonts");

	std::string fontFilename = GFX_DIRECTORY;
	fontFilename.append(FONT_FACE_FILENAME);

	m_regularFont = TTF_OpenFont(fontFilename.c_str(), REGULAR_FONT_SIZE_PT);
	if (m_regularFont == nullptr)
	{
		LOG_SCR_F("Unable to load regular font: %s\n", TTF_GetError());
		exit(1);
	}

	m_bigFont = TTF_OpenFont(fontFilename.c_str(), BIG_FONT_SIZE_PT);
	if (m_bigFont == nullptr)
	{
		LOG_SCR_F("Unable to load big font: %s\n", TTF_GetError());
		exit(1);
	}
}

void CApp::CleanUpFonts()
{
	LOG_SCR("Cleaning up Fonts");
	if (m_bigFont != nullptr)
	{
		TTF_CloseFont(m_bigFont);
		m_bigFont = nullptr;
	}
	if (m_regularFont != nullptr)
	{
		TTF_CloseFont(m_regularFont);
		m_regularFont = nullptr;
	}

	TTF_Quit();
}

#if SOUND_ENABLED
//********** FMOD SOUND LIBRARY INITIALIZATION / CLEANUP *********************************************************
void CApp::InitFMOD()
{
	LOG_SCR("Initializing FMOD");

	FMOD_RESULT result;
		
	// Create the main system object
	result = FMOD::System_Create(&m_fmodSystem);
	if (result != FMOD_OK)
	{
		LOG_SCR_F("Failed to create FMOD system object: %s\n", FMOD_ErrorString(result));
		exit(1);
	}

	// initialize FMOD
	result = m_fmodSystem->init(512, FMOD_INIT_NORMAL, nullptr);
	if (result != FMOD_OK)
	{
		LOG_SCR_F("Failed to initialize FMOD system object: %s\n", FMOD_ErrorString(result));
		exit(1);
	}
	
	// create a channel group
	result = m_fmodSystem->createChannelGroup("sfx", &m_channelGroup);
	if (result != FMOD_OK)
	{
		LOG_SCR_F("Failed to create FMOD sfx channel group: %s\n", FMOD_ErrorString(result));
		exit(1);
	}
}

void CApp::CleanUpFMOD()
{
	LOG_SCR("Cleaning up FMOD");

	m_channelGroup->release();
	m_fmodSystem->release();
}
#endif

//********** WRAPPERS TO LOAD SOUNDS *********************************************************

#if SOUND_ENABLED
FMOD_RESULT F_CALLBACK channelGroupCallback(FMOD_CHANNELCONTROL* channelControl,
	FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType,
	void* commandData1, void* commandData2)
{
	// for future expansion
	return FMOD_OK;
}

bool CApp::PrepareSoundChannel(FMOD::Channel* channel)
{
	// assign the FMOD channel to a FMOD channel group
	FMOD_RESULT result = channel->setChannelGroup(m_channelGroup);
	if (result != FMOD_OK)
	{
		LOG_SCR_F("Failed to set channel to group: %s\n", FMOD_ErrorString(result));
		return false;
	}

	// set callback on FMOD channel for post processing which is optional
	result = channel->setCallback(&channelGroupCallback);
	if (result != FMOD_OK)
	{
		LOG_SCR_F("Failed to set callback on channel: %s\n", FMOD_ErrorString(result));
		return false;
	}

	return true;
}

void CApp::SetChannelVolume(FMOD::Channel* channel, float volume)
{
	assert(channel != nullptr);

	channel->setVolume(volume);
}
#endif

//********** MAIN GAME LOOP *********************************************************

void CApp::HandleInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			exit(0);
			break;

		case SDL_KEYDOWN:
			m_gameManager.HandleKeyDownInput(&event.key);
			break;

		case SDL_KEYUP:
			m_gameManager.HandleKeyUpInput(&event.key);
			break;
		}
	}
}

void CApp::PrepareScene()
{
	SDL_RenderClear(m_renderer);
}

void CApp::PresentScene()
{
	SDL_RenderPresent(m_renderer);
}

void CApp::Update()
{
	// initialize the game manager
	m_gameManager.Init();

	Uint32 ticks = Utils::GetTicks();

	while (true)
	{
		Uint32 currentTicks = Utils::GetTicks();
		Uint32 elapsedTime = currentTicks - ticks;
		ticks = currentTicks;

		PrepareScene();
		HandleInput();
		m_gameManager.Update(elapsedTime);
		PresentScene();
		SDL_Delay(16);

#if SOUND_ENABLED
		// update fmod
		m_fmodSystem->update();
#endif
	}
}