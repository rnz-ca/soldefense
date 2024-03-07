/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "boss.h"

#include "ingamestate.h"
#include "utils.h"

SAnimationDef CBoss::m_animTable[CBoss::ANIM_TABLE_COUNT] =
{
	SAnimationDef(0, 406, 23, 100, SAnimationDef::EPlayMethod::LOOP), // SAUCER
	SAnimationDef(0, 501, 30, 75, SAnimationDef::EPlayMethod::LOOP), // SPIDER	
	SAnimationDef(0, 596, 25, 150, SAnimationDef::EPlayMethod::LOOP), // WALKER
};

CBoss::CBoss()
{
	m_type = EEntityType::BOSS;
}

void CBoss::Init(CTexture* spriteSheetTexture)
{
	CEntity::Init(spriteSheetTexture);
	SetIsAlive(false);
}

#if SOUND_ENABLED
void CBoss::InitSound(CSound* bossSpawnSound, CSound* bossNullifySound, CSound* bossEnhanceSound)
{
	m_spawnSound = bossSpawnSound;
	m_nullifySound = bossNullifySound;
	m_enhanceSound = bossEnhanceSound;
}
#endif

void CBoss::Destroy()
{
	// for future expansion
}

void CBoss::Spawn(EBossType bossType)
{
	if (m_state != EState::UNASSIGNED)
	{
		return;
	}

	int spriteWidth = 0;
	int spriteHeight = 0;
	EAnimID animId;
	float moveSpeedX = 0.0f;
	float moveSpeedY = 0.0f;
	Uint32 stayOnCenterTimeMs = 0;
	int pointsWorth = 0;

	if (bossType == EBossType::RANDOM)
	{
		bossType = static_cast<EBossType>(Utils::GetRandomUint32(static_cast<Uint32>(EBossType::SAUCER), static_cast<Uint32>(EBossType::WALKER)));
	}

	if (bossType == EBossType::SAUCER)
	{
		spriteWidth = SAUCER_SPRITE_WIDTH;
		spriteHeight = SAUCER_SPRITE_HEIGHT;
		animId = EAnimID::SAUCER_IDLE;
		moveSpeedX = SAUCER_MOVE_SPEED_X;
		moveSpeedY = SAUCER_MOVE_SPEED_Y;
		stayOnCenterTimeMs = SAUCER_STAY_ON_CENTER_TIME_MS;
		pointsWorth = SAUCER_POINTS_WORTH;
	}
	else if (bossType == EBossType::SPIDER)
	{
		spriteWidth = SPIDER_SPRITE_WIDTH;
		spriteHeight = SPIDER_SPRITE_HEIGHT;
		animId = EAnimID::SPIDER_IDLE;
		moveSpeedX = SPIDER_MOVE_SPEED_X;
		moveSpeedY = SPIDER_MOVE_SPEED_Y;
		stayOnCenterTimeMs = SPIDER_STAY_ON_CENTER_TIME_MS;
		pointsWorth = SPIDER_POINTS_WORTH;
	}
	else // EBossType::WALKER
	{
		spriteWidth = WALKER_SPRITE_WIDTH;
		spriteHeight = WALKER_SPRITE_HEIGHT;
		animId = EAnimID::WALKER_IDLE;
		moveSpeedX = WALKER_MOVE_SPEED_X;
		moveSpeedY = WALKER_MOVE_SPEED_Y;
		stayOnCenterTimeMs = WALKER_STAY_ON_CENTER_TIME_MS;
		pointsWorth = WALKER_POINTS_WORTH;
	}

	// scale the speed by the difficulty multiplier
	CIngameState* ingameState = dynamic_cast<CIngameState*>(CApp::GetInstance()->GetGameManager()->GetState());
	float difficultyMultiplier = ingameState->GetCurrentDifficultyMultiplier();

	SetSpriteWidth(spriteWidth);
	SetSpriteHeight(spriteHeight);
	SetMoveSpeedX(moveSpeedX * difficultyMultiplier);
	SetMoveSpeedY(moveSpeedY * difficultyMultiplier);
	m_stayOnCenterTimeMs = stayOnCenterTimeMs;

	// calculate initial position
	m_initialPosX = static_cast<float>(CApp::GetInstance()->GetScreenWidth()); // position it on the right edge of the screen
	m_initialPosY = static_cast<float>((BOSS_SPAWN_Y_LBOUND + BOSS_SPAWN_Y_UBOUND - spriteHeight) / 2.0f);
	SetPosition(m_initialPosX, m_initialPosY);

	// initialize animation
	m_animationMgr.Setup(m_animTable, spriteWidth, spriteHeight, m_spriteSheetTexture);
	m_animationMgr.RequestAnimation(static_cast<int>(animId));

	m_currentBossType = bossType;
	m_bossPointsWorth = pointsWorth;

	m_state = EState::ENTERING;

#if SOUND_ENABLED
	m_spawnSound->Play();
#endif

	SetIsAlive(true);
}

void CBoss::Despawn()
{
	if (!IsAlive() || m_state == EState::UNASSIGNED)
	{
		return;
	}

	m_currentBossType = EBossType::UNASSIGNED;
	SetIsAlive(false);
	m_state = EState::UNASSIGNED;
}

void CBoss::Update(Uint32 elapsedTime)
{
	if (m_state == EState::UNASSIGNED)
	{
		return;
	}

	float moveX = 0.0f;
	float moveY = 0.0f;

	if (m_state == EState::ENTERING || m_state == EState::LEAVING)
	{
		moveX = static_cast<float>(Utils::ScaleSpeed(elapsedTime, m_moveSpeedX));
		moveY = static_cast<float>(Utils::ScaleSpeed(elapsedTime, m_moveSpeedY));
	}

	m_x += moveX;
	m_y += moveY;

	if (m_state == EState::ENTERING)
	{
		// check if boss has reached the center of the screen horizontally
		float limit = (CApp::GetInstance()->GetScreenWidth() - GetSpriteWidth()) / 2.0f;
		if (m_x <= limit)
		{
			m_x = limit;
			m_state = EState::HOVER;
			m_lastHoverTicks = Utils::GetTicks();

#if SOUND_ENABLED
			if (CanBossNullifyPlayerShield(m_currentBossType))
			{
				m_nullifySound->Play();
			}
			else if (CanBossMakeEnemiesShootDiagonally(m_currentBossType))
			{
				m_enhanceSound->Play();
			}			
#endif
		}
	}
	else if (m_state == EState::HOVER)
	{
		if (Utils::GetTicks() - m_lastHoverTicks > m_stayOnCenterTimeMs)
		{
			m_state = EState::LEAVING;
			m_lastHoverTicks = Utils::GetTicks();
		}
	}
	else if (m_state == EState::LEAVING)
	{
		// check if boss has reached x = -sprite_width (left the screen)
		float limit = GetSpriteWidth() * -1.0f;
		if (m_x < limit)
		{
			m_x = limit;
			m_state = EState::UNASSIGNED;			
			SetIsAlive(false);

			CIngameState* ingameState = dynamic_cast<CIngameState*>(CApp::GetInstance()->GetGameManager()->GetState());
			ingameState->OnBossLeave();
		}
	}

	m_animationMgr.Update(elapsedTime);
}

void CBoss::Draw()
{
	if (m_state == EState::UNASSIGNED)
	{
		return;
	}

	m_animationMgr.Draw(static_cast<int>(m_x), static_cast<int>(m_y));
}

bool CBoss::CanBossNullifyPlayerShield(EBossType bossType)
{
	return bossType == EBossType::SPIDER;
}

bool CBoss::CanBossMakeEnemiesShootDiagonally(EBossType bossType)
{
	return bossType == EBossType::WALKER;
}