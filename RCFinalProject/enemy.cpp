/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "enemy.h"

#include "ingamestate.h"
#include "projectile.h"
#include "utils.h"

// Anim Table
SAnimationDef CEnemy::m_animTable[CEnemy::ANIM_TABLE_COUNT] = 
{
	SAnimationDef(0, 0, 15, 100, SAnimationDef::EPlayMethod::LOOP), // IDLE
	SAnimationDef(0, 136, 23, 100, SAnimationDef::EPlayMethod::LOOP), // SPIN
	SAnimationDef(0, 70, 6, 100, SAnimationDef::EPlayMethod::LOOP_BOOMERANG), // PROPULSION
};

CEnemy::CEnemy()
{
	m_type = EEntityType::ENEMY;

	SetSpriteWidth(SPRITE_WIDTH);
	SetSpriteHeight(SPRITE_HEIGHT);
	SetMoveSpeedX(ENEMY_MOVE_SPEED_X);
	SetMoveSpeedY(ENEMY_MOVE_SPEED_Y);
}

void CEnemy::Init(CEnemyFormation* enemyFormation, CTexture* spriteSheetTexture, Uint8 row, Uint8 column, float initialPosX, float initialPosY)
{
	CEntity::Init(spriteSheetTexture);

	m_spot = Utils::SGridLocation8{ row, column };
	m_initialPosX = initialPosX;
	m_initialPosY = initialPosY;
	m_enemyFormation = enemyFormation;

	// initialize animation
	m_animationMgr.Setup(m_animTable, SPRITE_WIDTH, SPRITE_HEIGHT, spriteSheetTexture);

	// generate a fire cooldown time
	GenerateFireCooldownTime();

	// set position
	SetPosition(initialPosX, initialPosY);

	// initialize the attack timer
	m_lastAttackTicks = Utils::GetTicks();
}
   
void CEnemy::Update(Uint32 elapsedTime)
{
	CIngameState* ingameState = dynamic_cast<CIngameState*>(CApp::GetInstance()->GetGameManager()->GetState());

	if (ingameState->GetState() == CIngameState::EState::PLAYING)
	{
		RequestAnimation(EAnimID::IDLE);

		float speedMultiplier = m_enemyFormation->GetSpeedMultiplier();

		float moveX = static_cast<float>(Utils::ScaleSpeed(elapsedTime, m_moveSpeedX * speedMultiplier) * m_enemyFormation->GetDirectionX());
		float moveY = static_cast<float>(Utils::ScaleSpeed(elapsedTime, m_moveSpeedY) * m_enemyFormation->GetDirectionY());

		m_x += moveX;
		m_y += moveY;

		if (m_canAttack && Utils::GetTicks() - m_lastAttackTicks > m_fireCooldownMs)
		{
			ShootProjectile();
			GenerateFireCooldownTime();
			m_lastAttackTicks = Utils::GetTicks();
		}
	}
	else if (ingameState->GetState() == CIngameState::EState::PLAYER_DEATH_COOLDOWN)
	{
		// make sure the right animation is being played
		RequestAnimation(EAnimID::PROPULSION);

		// only move vertically to the initial position, this is to give the player a chance when the player dies
		float moveY = static_cast<float>(Utils::ScaleSpeed(elapsedTime, ENEMY_RETURN_TO_INITIAL_POSITION_MOVE_SPEED_Y) * -1);

		m_y += moveY;
		if (m_y <= m_initialPosY)
		{
			m_y = m_initialPosY;
		}

		// if ingame state is on any other state, update the m_lastAttackTicks so that when the state is back to PLAYING,
		// all enemies shoot at once
		m_lastAttackTicks = Utils::GetTicks();
	}

	m_animationMgr.Update(elapsedTime);
}

void CEnemy::RequestAnimation(EAnimID animationIndex)
{
	if (m_animationMgr.GetRequestedAnimation() != static_cast<int>(animationIndex) && m_animationMgr.GetCurrentAnimation() != static_cast<int>(animationIndex))
	{
		m_animationMgr.RequestAnimation(static_cast<int>(animationIndex));
	}
}

void CEnemy::Draw()
{
	m_animationMgr.Draw(static_cast<int>(m_x), static_cast<int>(m_y));
}

void CEnemy::ShootProjectile()
{
	float projectilePosX = m_x + (m_spriteWidth / 2.0f) - (CProjectile::ENEMY_PROJECTILE_SPRITE_WIDTH / 2.0f);
	float projectilePosY = m_y + CProjectile::ENEMY_PROJECTILE_SPRITE_HEIGHT;

	CIngameState* ingameState = dynamic_cast<CIngameState*>(CApp::GetInstance()->GetGameManager()->GetState());
	CProjectile::EProjectileType projectileType = ingameState->IsBossMakingEnemiesShootDiagonally() ? CProjectile::EProjectileType::DIAGONAL : CProjectile::EProjectileType::REGULAR;
	ingameState->SpawnProjectile(CProjectile::EProjectileOwner::ENEMY, projectileType, projectilePosX, projectilePosY);
#if SOUND_ENABLED
	m_enemyFormation->GetAttackSound()->Play();
#endif

}

void CEnemy::GenerateFireCooldownTime()
{
	m_fireCooldownMs = Utils::GetRandomUint32(ENEMY_FIRE_COOLDOWN_LBOUND_MS, ENEMY_FIRE_COOLDOWN_UBOUND_MS);
	LOG_SCR_F("generated a random number of %ld\n", m_fireCooldownMs);
}