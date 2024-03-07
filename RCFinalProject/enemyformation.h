#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "texture.h"
#include "doublelinkedlist.h"
#include "enemy.h"
#include "preproc.h"
#include "sound.h"

class CEnemy;

class CEnemyFormation
{
public:
	static const int ENEMY_POINTS_WORTH = 70;

	enum class EState : int
	{
		NORMAL,
	};

	void InitTexture(CTexture* spriteSheetTexture);
#if SOUND_ENABLED
	void InitSound(CSound* enemyAttackSound);
#endif
	void Destroy();

	void Spawn();

	void Update(Uint32 elapsedTime);
	void Draw();

	CDoubleLinkedList<CEnemy*>& GetEntities() { return m_entitiesList; }

	int8_t GetDirectionX() const { return m_directionX; }
	int8_t GetDirectionY() const { return m_directionY; }
	float GetSpeedMultiplier() const { return m_speedMultiplier; }

	int GetEnemyCount() const { return m_enemyCount; }

	void OnEnemyDeath();

	EState GetState() const { return m_state; }

#if SOUND_ENABLED
	CSound* GetAttackSound() { return m_attackSound; }
#endif

	static const int SPRITE_WIDTH = 70;
	static const int SPRITE_HEIGHT = 70;

private:
	const float ENEMY_FORMATION_SPEED_INCREASE_MULTIPLIER = 1.68f;
	const float ENEMY_FORMATION_SPEED_INCREASE_THRESHOLD = 0.1f;
	static const int ENEMY_NUM_ENEMIES_PER_LINE = 11;
	const float ENEMY_INITIAL_POS_Y = 160.0f;
	const int ENEMY_NUM_LINES = 5;
	const int FORMATION_MOVE_LIMIT_Y = 600;
	const float FORMATION_VERTICAL_SPEED = -40.0f;

	void GetEnemiesInEdges(CEnemy** topmostEnemy, CEnemy** bottommostEnemy, CEnemy** leftmostEnemy, CEnemy** rightmostEnemy);
	void SetSpeedMultiplier();
	void UpdateFrontEnemiesTable();
	
	CTexture* m_spriteSheetTexture = nullptr;

	CDoubleLinkedList<CEnemy*> m_entitiesList;

	// table to store which enemies are in the front line for EACH column - index is column, value is row
	CEnemy* m_frontEnemiesTable[ENEMY_NUM_ENEMIES_PER_LINE] = {};

	float m_formationYMovePos = 0.0f;
	float m_speedMultiplier = 0.0f;
	EState m_state = EState::NORMAL;
	int m_totalEnemies = 0;
	int m_enemyCount = 0;
	int8_t m_previousDirectionX = 0;
	int8_t m_directionX = 0;
	int8_t m_directionY = 0;

#if SOUND_ENABLED
	CSound* m_attackSound;
#endif
};

