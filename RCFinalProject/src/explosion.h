#pragma once
#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "entity.h"
#include "preproc.h"
#include "projectile.h"
#if SOUND_ENABLED
#include "sound.h"
#endif

class CExplosion : public CEntity
{
public:
	// animation IDs
	enum class EAnimID : int
	{
		ENEMY_EXPLOSION = 0,
		PLAYER_EXPLOSION = 1,
	};

	CExplosion();

	void Init(CTexture* spriteSheetTexture, EAnimID animId, Uint32 explosionLifetimeMs);
#if SOUND_ENABLED
	void InitSound(CSound* sound);
#endif

	void Update(Uint32 elapsedTime);
	void Draw();

	void SetIsAlive(bool isAlive) { m_isAlive = isAlive; }

	static const int SPRITE_WIDTH = 90;
	static const int SPRITE_HEIGHT = 86;

private:
	// animation
	static const int ANIM_TABLE_COUNT = 2;
	static SAnimationDef m_animTable[ANIM_TABLE_COUNT];

#if SOUND_ENABLED
	CSound* m_sound = nullptr;
#endif

	float m_initialPosX = 0.0f;
	float m_initialPosY = 0.0f;
	int m_lastTicks = 0;
	Uint32 m_explosionLifetimeMs = 0;
};