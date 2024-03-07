#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#if __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

class CGameState
{
public:
	virtual void Init() = 0;
	virtual void CleanUp() = 0;
	virtual void HandleKeyDownInput(SDL_KeyboardEvent* kbEvent) = 0;
	virtual void HandleKeyUpInput(SDL_KeyboardEvent* kbEvent) = 0;
	virtual void Update(Uint32 elapsedTime) = 0;
	virtual void Draw() = 0;
	virtual void DrawDebug() {}

protected:
	bool IsInitialized() { return m_isInitialized; }
	void SetIsInitialized(bool isInitialized)
	{
		m_isInitialized = isInitialized;
	}

	bool m_isInitialized = false;
};