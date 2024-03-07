/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "playership.h"

#include "ingamestate.h"
#include "utils.h"

// Anim Tables
SAnimationDef CPlayerShip::m_animTable[CPlayerShip::ANIM_TABLE_COUNT] = 
{ 
	SAnimationDef(0, 0, 19, 50, SAnimationDef::EPlayMethod::LOOP)
}
;
SAnimationDef CPlayerShip::m_shieldAnimTable[CPlayerShip::SHIELD_ANIM_TABLE_COUNT] = 
{ 
	SAnimationDef(0, 297, 4, 100, SAnimationDef::EPlayMethod::LOOP)
};

CPlayerShip::CPlayerShip()
{
	m_type = EEntityType::PLAYERSHIP;

	SetSpriteWidth(SPRITE_WIDTH);
	SetSpriteHeight(SPRITE_HEIGHT);
	SetMoveSpeedX(MOVE_SPEED_X);
	SetMoveSpeedY(MOVE_SPEED_Y);
	SetColliderScaleFactor(COLLIDER_SCALE_FACTOR);
}

void CPlayerShip::Init(CTexture* spriteSheetTexture)
{
	CEntity::Init(spriteSheetTexture);

	// calculate initial position
	const float spawnYOffset = 80.0f;
	m_initialPosX = static_cast<float>(CApp::GetInstance()->GetScreenWidth() + GetSpriteWidth()) / 2.0f;
	m_initialPosY = static_cast<float>(CApp::GetInstance()->GetScreenHeight() - GetSpriteHeight() - spawnYOffset);
	SetPosition(m_initialPosX, m_initialPosY);

	// initialize animation
	m_animationMgr.Setup(m_animTable, SPRITE_WIDTH, SPRITE_HEIGHT, spriteSheetTexture);
	m_animationMgr.RequestAnimation(static_cast<int>(EAnimID::IDLE));
	m_shieldAnimationMgr.Setup(m_shieldAnimTable, SHIELD_SPRITE_WIDTH, SHIELD_SPRITE_HEIGHT, spriteSheetTexture);
	m_shieldAnimationMgr.RequestAnimation(static_cast<int>(EShieldAnimID::IDLE));
}

#if SOUND_ENABLED
void CPlayerShip::InitSound(CSound* shootSound, CSound* shieldSound, CSound* shieldNullifiedSound)
{
	m_shootSound = shootSound;
	m_shieldSound = shieldSound;
	m_shieldNullifiedSound = shieldNullifiedSound;
}
#endif

void CPlayerShip::Update(Uint32 elapsedTime)
{
	if (CanMove())
	{
		int moveModifierX = 0;
		if (m_inputData.m_left == 1)
		{
			moveModifierX = -1;
		}
		else if (m_inputData.m_right == 1)
		{
			moveModifierX = 1;
		}

		int moveModifierY = 0;
		if (m_inputData.m_up == 1)
		{
			moveModifierY = -1;
		}
		else if (m_inputData.m_down == 1)
		{
			moveModifierY = 1;
		}

		if (moveModifierX != 0)
		{
			int moveX = static_cast<int>(Utils::ScaleSpeed(elapsedTime, m_moveSpeedX) * moveModifierX);
			m_x += moveX;

			// make sure the player does not go out of bounds
			if (m_x < 0)
			{
				m_x = 0;
			}
			else if (m_x > CApp::GetInstance()->GetScreenWidth() - SPRITE_WIDTH)
			{
				m_x = static_cast<float>(CApp::GetInstance()->GetScreenWidth()) - SPRITE_WIDTH;
			}
		}

		if (moveModifierY != 0)
		{
			int moveY = static_cast<int>(Utils::ScaleSpeed(elapsedTime, m_moveSpeedY) * moveModifierY);
			m_y += moveY;

			// make sure the player does not go out of bounds
			if (m_y < MOVE_LIMIT_Y)
			{
				m_y = MOVE_LIMIT_Y;
			}
			else if (m_y > CApp::GetInstance()->GetScreenHeight() - SPRITE_HEIGHT)
			{
				m_y = static_cast<float>(CApp::GetInstance()->GetScreenHeight()) - SPRITE_HEIGHT;
			}
		}

		if (m_inputData.m_fire == 1)
		{
			if (Utils::GetTicks() - m_lastShootTicks > FIRE_COOLDOWN_MS)
			{
				ShootProjectile();
				m_lastShootTicks = Utils::GetTicks();
			}
		}

		if (m_inputData.m_shield == 1)
		{
			ActivateShield();
		}
	}	

	UpdateShield();

	m_animationMgr.Update(elapsedTime);

	if (m_state == EState::USING_SHIELD)
	{
		m_shieldAnimationMgr.Update(elapsedTime);
	}
}

void CPlayerShip::Draw()
{
	if (m_isAlive)
	{
		// draw ship
		m_animationMgr.Draw(static_cast<int>(m_x), static_cast<int>(m_y));

		if (m_state == EState::USING_SHIELD)
		{
			// draw shield if active
			float shieldPosX = m_x + m_spriteWidth / 2.0f - SHIELD_SPRITE_WIDTH / 2.0f;
			float shieldPosY = m_y + m_spriteHeight / 2.0f - SHIELD_SPRITE_HEIGHT / 2.0f;
			m_shieldAnimationMgr.Draw(static_cast<int>(shieldPosX), static_cast<int>(shieldPosY));
		}
	}
}

void CPlayerShip::ShootProjectile()
{	
	float projectilePosX = m_x + (m_spriteWidth / 2.0f) - (CProjectile::PLAYER_PROJECTILE_SPRITE_WIDTH / 2.0f);
	float projectilePosY = m_y - CProjectile::PLAYER_PROJECTILE_SPRITE_HEIGHT;

	CIngameState* ingameState = dynamic_cast<CIngameState*>(CApp::GetInstance()->GetGameManager()->GetState());
	ingameState->SpawnProjectile(CProjectile::EProjectileOwner::PLAYER, CProjectile::EProjectileType::REGULAR, projectilePosX, projectilePosY);
#if SOUND_ENABLED
	m_shootSound->Play();
#endif
}

void CPlayerShip::ActivateShield()
{
	CIngameState* ingameState = dynamic_cast<CIngameState*>(CApp::GetInstance()->GetGameManager()->GetState());

	if (ingameState->IsBossNullifyingPlayerShield())
	{
#if SOUND_ENABLED
		m_shieldNullifiedSound->Play(SHIELD_NULLIFIED_SOUND_VOLUME);
#endif
	}
	else if (m_state == EState::NORMAL)
	{
		m_shieldStartTicks = Utils::GetTicks();
		m_state = EState::USING_SHIELD;
#if SOUND_ENABLED
		m_shieldSound->Play();
#endif
	}
}

void CPlayerShip::UpdateShield()
{
	if (m_state == EState::USING_SHIELD)
	{
		if (Utils::GetTicks() - m_shieldStartTicks > SHIELD_DURATION)
		{
			m_state = EState::NORMAL;
		}
	}
}

void CPlayerShip::Respawn()
{
	// put the player back in its initial position
	SetPosition(m_initialPosX, m_initialPosY);
	SetIsAlive(true);

	m_state = EState::NORMAL;
}

void CPlayerShip::OnCollision()
{
	if (m_state != EState::USING_SHIELD)
	{
		SetIsAlive(false);
	}
}