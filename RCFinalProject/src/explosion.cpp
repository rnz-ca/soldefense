/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "explosion.h"

#include "ingamestate.h"
#include "utils.h"

// Anim Table
SAnimationDef CExplosion::m_animTable[CExplosion::ANIM_TABLE_COUNT] = 
{ 
	SAnimationDef(0, 773, 31, 5, SAnimationDef::EPlayMethod::NORMAL),
	SAnimationDef(0, 773, 31, 42, SAnimationDef::EPlayMethod::NORMAL)
};

CExplosion::CExplosion()
{
	m_type = EEntityType::EXPLOSION;

	SetSpriteWidth(SPRITE_WIDTH);
	SetSpriteHeight(SPRITE_HEIGHT);
}

void CExplosion::Init(CTexture* spriteSheetTexture, EAnimID animId, Uint32 explosionLifetimeMs)
{
	CEntity::Init(spriteSheetTexture);

	// calculate initial position
	m_initialPosX = static_cast<float>(CApp::GetInstance()->GetScreenWidth() + GetSpriteWidth()) / 2.0f;
	m_initialPosY = static_cast<float>(CApp::GetInstance()->GetScreenHeight() - GetSpriteHeight()) / 2.0f;
	SetPosition(m_initialPosX, m_initialPosY);

	// initialize animation
	m_animationMgr.Setup(m_animTable, SPRITE_WIDTH, SPRITE_HEIGHT, spriteSheetTexture);
	m_animationMgr.RequestAnimation(static_cast<int>(animId));
		
	m_explosionLifetimeMs = explosionLifetimeMs;

	m_lastTicks = Utils::GetTicks();
}

#if SOUND_ENABLED
void CExplosion::InitSound(CSound* sound)
{
	m_sound = sound;
	m_sound->Play();
}
#endif

void CExplosion::Update(Uint32 elapsedTime)
{
	if (Utils::GetTicks() - m_lastTicks > m_explosionLifetimeMs)
	{
		SetIsAlive(false);
	}

	m_animationMgr.Update(elapsedTime);
}

void CExplosion::Draw()
{
	m_animationMgr.Draw(static_cast<int>(m_x), static_cast<int>(m_y));
}