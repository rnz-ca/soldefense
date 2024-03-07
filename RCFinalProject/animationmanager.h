#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#if __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#include "app.h"
#include "texture.h"

struct SAnimationDef
{
	enum class EPlayMethod : int
	{
		NORMAL,
		LOOP,
		LOOP_BOOMERANG
	};

	int m_startPosX = 0;
	int m_startPosY = 0;
	int m_numFrames = 0;
	Uint32 m_time = 0;
	EPlayMethod m_playMethod = EPlayMethod::NORMAL;

	SAnimationDef(int startPosX, int startPosY, int numFrames, Uint32 time, EPlayMethod playMethod)
	{
		m_startPosX = startPosX;
		m_startPosY = startPosY;
		m_numFrames = numFrames;
		m_time = time;
		m_playMethod = playMethod;
	}
};

class CAnimationManager
{
public:
	static const int ANIMATION_NONE = -1;

	void Setup(const SAnimationDef* animTable, int frameWidth, int frameHeight, CTexture* spriteSheetTexture);
	void Setup(const SAnimationDef* animTable, int frameWidth, int frameHeight);

	void RequestAnimation(int index);

	void Update(Uint32 elapsedTime);
	void Draw(int posX, int posY, double angleInDegrees = 0.0f);

	int GetCurrentFrame() { return m_currentFrame; }
	int GetRequestedAnimation() { return m_requestedAnimIndex; }
	int GetCurrentAnimation() { return m_currentAnimIndex; }

private:
	void VerifyInitialization();

	int m_currentAnimIndex = ANIMATION_NONE;
	int m_requestedAnimIndex = ANIMATION_NONE;
	bool m_isInitialized = false;
	Uint32 m_ticks = 0;
	int m_currentFrame = 0;
	int8_t m_playDirection = 1; // only used for LOOP_BOOMERANG (-1 means backwards, 1 means forward)

	const SAnimationDef* m_animTable = nullptr;
	int m_frameWidth = 0;
	int m_frameHeight = 0;
	CTexture* m_spriteSheetTexture = nullptr;
};