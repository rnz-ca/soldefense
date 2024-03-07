#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "entity.h"
#if SOUND_ENABLED
#include "sound.h"
#endif

class CBoss : public CEntity
{
public:
	// animation IDs
	enum class EAnimID : int
	{
		SAUCER_IDLE,
		SPIDER_IDLE,
		WALKER_IDLE
	};

	enum class EBossType : int
	{
		UNASSIGNED,
		SAUCER,
		SPIDER,
		WALKER,
		RANDOM
	};

	enum class EState : int
	{
		UNASSIGNED,
		ENTERING,
		HOVER,
		LEAVING
	};

	// general
	static const int BOSS_SPAWN_Y_LBOUND = 60;
	static const int BOSS_SPAWN_Y_UBOUND = 155;

	// saucer
	static const int SAUCER_SPRITE_WIDTH = 128;
	static const int SAUCER_SPRITE_HEIGHT = 95;
	const float SAUCER_MOVE_SPEED_X = -45.0f;
	const float SAUCER_MOVE_SPEED_Y = 0.0f;
	const Uint32 SAUCER_STAY_ON_CENTER_TIME_MS = 3000;
	const int SAUCER_POINTS_WORTH = 500;

	// spider
	static const int SPIDER_SPRITE_WIDTH = 90;
	static const int SPIDER_SPRITE_HEIGHT = 94;
	const float SPIDER_MOVE_SPEED_X = -42.5f;
	const float SPIDER_MOVE_SPEED_Y = 0.0f;
	const Uint32 SPIDER_STAY_ON_CENTER_TIME_MS = 4000;
	const int SPIDER_POINTS_WORTH = 750;

	// walker
	static const int WALKER_SPRITE_WIDTH = 90;
	static const int WALKER_SPRITE_HEIGHT = 95;
	const float WALKER_MOVE_SPEED_X = -40.0f;
	const float WALKER_MOVE_SPEED_Y = 0.0f;
	const Uint32 WALKER_STAY_ON_CENTER_TIME_MS = 5000;
	const int WALKER_POINTS_WORTH = 1000;

	CBoss();

	void Init(CTexture* spriteSheetTexture);
#if SOUND_ENABLED
	void InitSound(CSound* bossSpawnSound, CSound* bossNullifySound, CSound* bossEnhanceSound);
#endif
	void Destroy();

	void Spawn(EBossType bossType);
	void Despawn();

	void Update(Uint32 elapsedTime);
	void Draw();

	float GetInitialPosX() const { return m_initialPosX; }
	float GetInitialPosY() const { return m_initialPosY; }

	EState GetState() const { return m_state; }

	bool IsNullifyingPlayerShield() { return m_state == EState::HOVER && CanBossNullifyPlayerShield(m_currentBossType); }
	bool IsMakingEnemiesShootDiagonally() { return m_state == EState::HOVER && CanBossMakeEnemiesShootDiagonally(m_currentBossType); }
	int GetPointsWorth() { return m_bossPointsWorth; }

	void OnLeave();
	
private:
	bool CanBossNullifyPlayerShield(EBossType bossType);
	bool CanBossMakeEnemiesShootDiagonally(EBossType bossType);
	
	// animation
	static const int ANIM_TABLE_COUNT = 3;
	static SAnimationDef m_animTable[ANIM_TABLE_COUNT];

	float m_initialPosX = 0;
	float m_initialPosY = 0;

	EBossType m_currentBossType = EBossType::UNASSIGNED;
	EState m_state = EState::UNASSIGNED;
	
	Uint32 m_lastHoverTicks = 0;
	Uint32 m_lastBossSpawnTicks = 0;
	Uint32 m_stayOnCenterTimeMs = 0;

	int m_bossPointsWorth = 0;

#if SOUND_ENABLED
	CSound* m_spawnSound = nullptr;
	CSound* m_nullifySound = nullptr;
	CSound* m_enhanceSound = nullptr;
#endif
};