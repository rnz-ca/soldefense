#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#if __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "gamestate.h"

class CGameManager
{
public:
	enum class EGameState : int
	{
		UNASSIGNED,
		INTRO,
		INGAME
	};

	void Init();
	void Destroy();

	void Update(Uint32 elapsedTime);
	void RequestState(EGameState state);

	static const EGameState INITIAL_STATE = EGameState::INTRO;

	void HandleKeyDownInput(SDL_KeyboardEvent* kbEvent);
	void HandleKeyUpInput(SDL_KeyboardEvent* kbEvent);

	CGameState* GetState() { return m_stateObj; }
	
	void ToggleIsBackgroundScrollingEnabled() { m_isBackgroundScrollingEnabled = !m_isBackgroundScrollingEnabled; }
	bool IsBackgroundScrollingEnabled() { return m_isBackgroundScrollingEnabled; }
	
private:
	EGameState m_currentState = EGameState::UNASSIGNED;
	EGameState m_requestedState = EGameState::UNASSIGNED;

	CGameState* m_stateObj = nullptr;

	bool m_isBackgroundScrollingEnabled = true;

	Uint8 m_toggleBackgroundScrollingKeyPressed = 0;
};