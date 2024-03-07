#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "gamestate.h"
#include "preproc.h"
#include <string>
#if SOUND_ENABLED
#include "sound.h"
#endif
#include "texture.h"

class CIntroState : public CGameState
{
public:
	enum class EState : int
	{
		UNASSIGNED,
		ANIMATE_LASER,
		DRAW_TITLE,
		IDLE,
	};

	void Init();
	void CleanUp();

	void HandleKeyDownInput(SDL_KeyboardEvent* kbEvent);
	void HandleKeyUpInput(SDL_KeyboardEvent* kbEvent);

	void InitValues();
#if SOUND_ENABLED
	void InitSounds();
#endif
	void InitTextures();
	void InitText();

#if SOUND_ENABLED
	void DestroySounds();
#endif
	void DestroyTextures();
	void DestroyText();

	void Update(Uint32 elapsedTime);
	void UpdateText(Uint32 elapsedTime);

	void Draw();
	void DrawText();

#if DEBUG_DRAW
	void DrawDebug() override;
#endif

private:
	struct SInputData
	{
		Uint8 m_anyKey;

		SInputData()
		{
			Reset();
		}

		void Reset()
		{
			m_anyKey = 0;
		}
	};

	// textures
	const std::string TEXTURE_SPRITESHEET_FILENAME = "introscreen_spritesheet.png";

	// sounds
	const std::string SOUND_MUSIC_FILENAME = "mus-menu.mod";
	const float SOUND_MUSIC_VOLUME = 0.75f;

	// labels
	const std::string LABEL_HIT_KEY_TO_START_TEXT = "HIT A KEY TO START!";
	const float LABEL_HIT_KEY_TO_START_POS_Y = 650.0f;
	const std::string LABEL_COPYRIGHT_TEXT = "2024 (C) ";
	const std::string LABEL_COPYRIGHT_TEXT_2 = " | ";
	const float LABEL_COPYRIGHT_POS_Y = 980.0f;
	const std::string LABEL_HIT_KEY_TO_TOGGLE_BACKGROUND_SCROLLING_TEXT = "<F9> TOGGLE BACKGROUND SCROLLING";
	const float LABEL_HIT_KEY_TO_TOGGLE_BACKGROUND_SCROLLING_POS_Y = 740.0f;
	const SDL_Point LABEL_COPYRIGHT_POS{ 50, 650 };
	const SDL_Color LABEL_RED_COLOR{ 255, 0, 0, 255 };
	const SDL_Color LABEL_YELLOW_COLOR{ 255, 255, 0, 255 };
	const SDL_Color LABEL_CYAN_COLOR{ 0, 255, 255, 255 };

	const EState INITIAL_STATE = EState::ANIMATE_LASER;

	// background texture movement
	const float BACKGROUND_STARTING_Y = 1080.0f * 2.0f;
	const float BACKGROUND_MOVE_SPEED = -36.0f;
	
	// laser texture movement
	SDL_Rect LASER_SPRITESHEET_RECT{ 0, 540, 623, 1080 };
	const float LASER_STARTING_Y = 1080.0f;
	const float LASER_MOVE_SPEED = -640.0f;

	// title texture
	SDL_Rect TITLE_SPRITESHEET_RECT{ 0, 0, 725, 458 };
	const float TITLE_POS_Y = 60.0f;

	// text draw with box
	const SDL_Color BOX_FOREGROUND_COLOR{ 124, 0, 15, 255 };
	const SDL_Color BOX_BACKGROUND_COLOR{ 66, 3, 10, 255 };
	const int BOX_TEXT_SPACING = 16;
	const Uint32 HIT_KEY_LABEL_COLOR_SWITCH_TIME_MS = 64;

	void RequestState(EState state);
	void SetInputAnyKey(Uint8 v) { m_inputData.m_anyKey = v; }
	void DrawCenteredTextLabel(CTexture& texture, int posY, bool drawBox);
	
	CTexture m_spriteSheetTexture;
	CTexture m_copyrightLabelTexture;
	CTexture m_hitKeyToStartLabelTexture;
	CTexture m_hitKeyToToggleBackgroundScrollingTexture;

#if SOUND_ENABLED
	CSound m_music;
#endif

	EState m_currentState = EState::UNASSIGNED;
	EState m_requestedState = EState::UNASSIGNED;

	Uint32 m_lastBossSpawnTicks = 0;
	Uint32 m_lastHitKeyLabelColorSwitchTicks = 0;
	Uint8 m_originalHitKeyLabelTintRed = 0;
	Uint8 m_originalHitKeyLabelTintGreen = 0;
	Uint8 m_originalHitKeyLabelTintBlue = 0;
	bool m_hitKeyLabelColorSwitchToggle = false;

	float m_backgroundPosY = 0.0f;
	float m_laserPosY = 0.0f;

	SInputData m_inputData;

	int m_previousLives = -1;
};