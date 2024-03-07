#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "entity.h"
#include "enemyformation.h"
#include "utils.h"

class CEnemyFormation;

class CEnemy : public CEntity
{
public:
	// animation IDs
	enum class EAnimID : int
	{
		IDLE = 0,
		SPIN = 1,
		PROPULSION = 2,
	};

	CEnemy();

	void Init(CEnemyFormation* enemyFormation, CTexture* spriteSheetTexture, Uint8 row, Uint8 column, float initialPosX, float initialPosY);

	void Update(Uint32 elapsedTime);
	void Draw();

	void ShootProjectile();

	const Utils::SGridLocation8& GetSpot() const { return m_spot; }
	float GetInitialPosX() { return m_initialPosX; }
	float GetInitialPosY() { return m_initialPosY; }
	bool IsAtInitialPosY() { return m_y == m_initialPosY; }
	void SetCanAttack(bool canAttack) { m_canAttack = canAttack; }
	void SetDistanceToPlayer(float distanceToPlayer) { m_distanceToPlayer = distanceToPlayer; }

	static const int SPRITE_WIDTH = 70;
	static const int SPRITE_HEIGHT = 70;
	
private:
	const float ENEMY_RETURN_TO_INITIAL_POSITION_MOVE_SPEED_Y = 32.0f;
	const Uint32 ENEMY_FIRE_COOLDOWN_LBOUND_MS = 2000;
	const Uint32 ENEMY_FIRE_COOLDOWN_UBOUND_MS = 7000;

	// animation
	static const int ANIM_TABLE_COUNT = 3;
	static SAnimationDef m_animTable[ANIM_TABLE_COUNT];

	void RequestAnimation(EAnimID animationIndex);
	void GenerateFireCooldownTime();

	CEnemyFormation* m_enemyFormation = nullptr;
	float m_initialPosX = 0;
	float m_initialPosY = 0;
	float m_distanceToPlayer = 0.0f;
	int8_t m_directionX = 1;
	int8_t m_directionY = 0;
	Utils::SGridLocation8 m_spot;
	bool m_canAttack = false;
	Uint32 m_lastAttackTicks = 0;
	Uint32 m_fireCooldownMs = 0;
};