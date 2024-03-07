/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "gamemanager.h"

#include "ingamestate.h"
#include "introstate.h"
#include <assert.h>

void CGameManager::Init()
{
	RequestState(INITIAL_STATE);
}

void CGameManager::Destroy()
{
	// destroy the current state
	if (m_stateObj != nullptr)
	{
		m_stateObj->CleanUp();
		delete m_stateObj;
		m_stateObj = nullptr;
	}
}

void CGameManager::RequestState(EGameState state)
{
	m_requestedState = state;
}

void CGameManager::HandleKeyDownInput(SDL_KeyboardEvent* kbEvent)
{
	if (m_stateObj == nullptr)
	{
		return;
	}

	if (kbEvent->repeat != 0)
	{
		return;
	}

	switch (kbEvent->keysym.scancode)
	{
	case SDL_SCANCODE_F9:
		m_toggleBackgroundScrollingKeyPressed = 1;
		break;

	default:
		m_stateObj->HandleKeyDownInput(kbEvent);
		break;
	}
}

void CGameManager::HandleKeyUpInput(SDL_KeyboardEvent* kbEvent)
{
	if (m_stateObj == nullptr)
	{
		return;
	}

	if (kbEvent->repeat != 0)
	{
		return;
	}

	switch (kbEvent->keysym.scancode)
	{
	case SDL_SCANCODE_F9:
		m_toggleBackgroundScrollingKeyPressed = 0;
		break;

	default:
		m_stateObj->HandleKeyUpInput(kbEvent);
		break;
	}
}

void CGameManager::Update(Uint32 elapsedTime)
{
	if (m_toggleBackgroundScrollingKeyPressed == 1)
	{
		ToggleIsBackgroundScrollingEnabled();
		m_toggleBackgroundScrollingKeyPressed = 0;
	}

	if (m_requestedState != EGameState::UNASSIGNED)
	{
		assert(m_requestedState != m_currentState);

		// destroy the previous state
		Destroy();
		
		// create the new state
		if (m_requestedState == EGameState::INTRO)
		{
			m_stateObj = new CIntroState(); // polymorphism
		}
		else if (m_requestedState == EGameState::INGAME)
		{
			m_stateObj = new CIngameState(); // polymorphism
		}

		m_currentState = m_requestedState;
		m_requestedState = EGameState::UNASSIGNED;

		m_stateObj->Init();
	}

	// there is always one state playing, no need to check for if nullptr
	m_stateObj->Update(elapsedTime);
	m_stateObj->Draw();
#if DEBUG_DRAW
	m_stateObj->DrawDebug();
#endif
}