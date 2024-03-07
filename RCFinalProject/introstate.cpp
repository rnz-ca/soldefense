/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "introstate.h"

#include "app.h"
#include <assert.h>
#include "utils.h"

void CIntroState::Init()
{
	InitValues();
#if SOUND_ENABLED
	InitSounds();
#endif	
	InitTextures();
	InitText();

	m_isInitialized = true;

#if SOUND_ENABLED
	// start playing the music
	m_music.SetLoop(true);
	m_music.Play(SOUND_MUSIC_VOLUME);
#endif

	RequestState(INITIAL_STATE);
}

void CIntroState::CleanUp()
{
	if (IsInitialized())
	{
		DestroyText();
		DestroyTextures();
#if SOUND_ENABLED
		DestroySounds();
#endif
	}
}

void CIntroState::RequestState(EState state)
{
	assert(m_currentState != state);
	m_requestedState = state;
}

void CIntroState::HandleKeyDownInput(SDL_KeyboardEvent* kbEvent)
{
	if (kbEvent->repeat != 0)
	{
		return;
	}

	switch (kbEvent->keysym.scancode)
	{
		case SDL_SCANCODE_ESCAPE:
			exit(0);
			break;

		default:
			m_inputData.m_anyKey = 1;
			break;
	}
}

void CIntroState::HandleKeyUpInput(SDL_KeyboardEvent* kbEvent)
{
	if (kbEvent->repeat != 0)
	{
		return;
	}

	m_inputData.m_anyKey = 0;
}

void CIntroState::InitValues()
{
	SetIsInitialized(false);

	m_backgroundPosY = BACKGROUND_STARTING_Y;
	m_laserPosY = LASER_STARTING_Y;
}

#if SOUND_ENABLED
void CIntroState::InitSounds()
{
	m_music.CreateFromFile(SOUND_MUSIC_FILENAME.c_str());
}

void CIntroState::DestroySounds()
{
	m_music.Destroy();
}
#endif

void CIntroState::Update(Uint32 elapsedTime)
{
	// handle state requests
	if (m_requestedState != EState::UNASSIGNED)
	{
		m_currentState = m_requestedState;
		m_requestedState = EState::UNASSIGNED;
	}

	// always move the background
	if (CApp::GetInstance()->GetGameManager()->IsBackgroundScrollingEnabled())
	{
		m_backgroundPosY += Utils::ScaleSpeed(elapsedTime, BACKGROUND_MOVE_SPEED);
		if (m_backgroundPosY <= 0.0f)
		{
			m_backgroundPosY = BACKGROUND_STARTING_Y; // wrap around
		}
	}
	

	if (m_currentState == EState::ANIMATE_LASER)
	{
		// move laser from bottom to top
		m_laserPosY += Utils::ScaleSpeed(elapsedTime, LASER_MOVE_SPEED);
		if (m_laserPosY <= -LASER_SPRITESHEET_RECT.h)
		{
			RequestState(EState::DRAW_TITLE);
			m_laserPosY = static_cast<float>(LASER_SPRITESHEET_RECT.h);
		}

	}
	else if (m_currentState == EState::DRAW_TITLE)
	{
		m_lastHitKeyLabelColorSwitchTicks = Utils::GetTicks();
		RequestState(EState::IDLE);
	}
	else if (m_currentState == EState::IDLE)
	{
		// for future expansion
	}

	if (m_inputData.m_anyKey == 1)
	{
		// request gamemanager to switch state
		CApp::GetInstance()->GetGameManager()->RequestState(CGameManager::EGameState::INGAME);
	}

	UpdateText(elapsedTime);
}

void CIntroState::UpdateText(Uint32 elapsedTime)
{
	// switch the text color
	if (Utils::GetTicks() - m_lastHitKeyLabelColorSwitchTicks > HIT_KEY_LABEL_COLOR_SWITCH_TIME_MS)
	{
		m_hitKeyLabelColorSwitchToggle = !m_hitKeyLabelColorSwitchToggle;
		if (m_hitKeyLabelColorSwitchToggle)
		{
			m_hitKeyToStartLabelTexture.SetTint(LABEL_RED_COLOR.r, LABEL_RED_COLOR.g, LABEL_RED_COLOR.b);
		}
		else
		{
			m_hitKeyToStartLabelTexture.SetTint(m_originalHitKeyLabelTintRed, m_originalHitKeyLabelTintGreen, m_originalHitKeyLabelTintBlue);
		}
		m_lastHitKeyLabelColorSwitchTicks = Utils::GetTicks();
	}
}

void CIntroState::Draw()
{
	CApp* app = CApp::GetInstance();
	int screenWidth = app->GetScreenWidth();
	int screenHeight = app->GetScreenHeight();

	// draw background
	SDL_Rect backgroundRect{ 1080, static_cast<int>(m_backgroundPosY), screenWidth, screenHeight };
	m_spriteSheetTexture.Draw(0, 0, &backgroundRect);

	// draw laser
	if (m_currentState >= EState::ANIMATE_LASER)
	{
		m_spriteSheetTexture.Draw(((screenWidth - LASER_SPRITESHEET_RECT.w) / 2), static_cast<int>(m_laserPosY), &LASER_SPRITESHEET_RECT);
	}

	// draw title
	if (m_currentState >= EState::DRAW_TITLE)
	{
		m_spriteSheetTexture.Draw((screenWidth - TITLE_SPRITESHEET_RECT.w) / 2, static_cast<int>(TITLE_POS_Y), &TITLE_SPRITESHEET_RECT);
	}
	
	if (m_currentState == EState::IDLE)
	{
		DrawText();
	}
}

void CIntroState::DrawCenteredTextLabel(CTexture& texture, int posY, bool drawBox)
{
	CApp* app = CApp::GetInstance();
	int screenWidth = app->GetScreenWidth();
	int halfScreenWidth = screenWidth / 2;
	int halfBoxTextSpacing = BOX_TEXT_SPACING / 2;
	int textureWidth = texture.GetWidth();

	int posX = (screenWidth - textureWidth) / 2;
	if (drawBox)
	{		
		app->DrawBox(posX - halfBoxTextSpacing, posY - halfBoxTextSpacing,
			textureWidth + BOX_TEXT_SPACING,
			texture.GetHeight() + BOX_TEXT_SPACING,
			true, 
			BOX_FOREGROUND_COLOR, 
			BOX_BACKGROUND_COLOR);
	}
	
	texture.Draw(posX, posY);
}

void CIntroState::DrawText()
{
	CApp* app = CApp::GetInstance();
	int screenWidth = app->GetScreenWidth();
	int halfScreenWidth = screenWidth / 2;
	
	DrawCenteredTextLabel(m_hitKeyToStartLabelTexture, static_cast<int>(LABEL_HIT_KEY_TO_START_POS_Y), false);
	DrawCenteredTextLabel(m_hitKeyToToggleBackgroundScrollingTexture, static_cast<int>(LABEL_HIT_KEY_TO_TOGGLE_BACKGROUND_SCROLLING_POS_Y), false);
	DrawCenteredTextLabel(m_copyrightLabelTexture, static_cast<int>(LABEL_COPYRIGHT_POS_Y), false);
}

void CIntroState::InitTextures()
{
	m_spriteSheetTexture.CreateFromFile(TEXTURE_SPRITESHEET_FILENAME);
}

void CIntroState::DestroyTextures()
{
	m_spriteSheetTexture.Destroy();
}

void CIntroState::InitText()
{
	m_hitKeyToStartLabelTexture.CreateFromText(LABEL_HIT_KEY_TO_START_TEXT.c_str(), LABEL_YELLOW_COLOR);
	m_hitKeyToStartLabelTexture.GetTint(&m_originalHitKeyLabelTintRed, &m_originalHitKeyLabelTintGreen, &m_originalHitKeyLabelTintBlue);

	m_hitKeyToToggleBackgroundScrollingTexture.CreateFromText(LABEL_HIT_KEY_TO_TOGGLE_BACKGROUND_SCROLLING_TEXT.c_str(), LABEL_CYAN_COLOR);

	std::string copyrightText = LABEL_COPYRIGHT_TEXT;
	copyrightText.append(APP_AUTHOR);
	copyrightText.append(LABEL_COPYRIGHT_TEXT_2);
	copyrightText.append(APP_ADDITIONAL_TEXT);
	m_copyrightLabelTexture.CreateFromText(copyrightText.c_str(), LABEL_YELLOW_COLOR);
}

void CIntroState::DestroyText()
{
	m_copyrightLabelTexture.Destroy();
	m_hitKeyToToggleBackgroundScrollingTexture.Destroy();
	m_hitKeyToStartLabelTexture.Destroy();
}

#if DEBUG_DRAW
void CIntroState::DrawDebug()
{
	// for future expansion
}
#endif