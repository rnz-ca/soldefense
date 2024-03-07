#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "entity.h"
#include "texture.h"

class CProjectile : public CEntity
{
public:
	// animation IDs
	enum class EAnimID : int
	{
		PLAYER_IDLE = 0,
		ENEMY_IDLE = 1,
	};

	enum class EProjectileOwner : int
	{
		UNASSIGNED,
		PLAYER,
		ENEMY
	};

	enum class EProjectileType : int
	{
		UNASSIGNED,
		REGULAR,
		DIAGONAL
	};

	CProjectile();

	void Init(EProjectileOwner owner, EProjectileType projectileType, CTexture* spriteSheetTexture, float x, float y);

	void Update(Uint32 elapsedTime);
	void Draw();

	void OnCollision();

	EProjectileOwner GetOwner() { return m_owner; }

	static const int PLAYER_PROJECTILE_SPRITE_WIDTH = 21;
	static const int PLAYER_PROJECTILE_SPRITE_HEIGHT = 40;
	const float PLAYER_PROJECTILE_MOVE_SPEED_X = 0.0f;
	const float PLAYER_PROJECTILE_MOVE_SPEED_Y = -144.0f;
	const int PLAYER_PROJECTILE_SPAWN_Y_OFFSET = -80;

	static const int ENEMY_PROJECTILE_SPRITE_WIDTH = 55;
	static const int ENEMY_PROJECTILE_SPRITE_HEIGHT = 60;
	const float ENEMY_PROJECTILE_MOVE_SPEED = 110;
	const int ENEMY_PROJECTILE_SPAWN_Y_OFFSET = 80;
	
private:
	// animation
	static const int ANIM_TABLE_COUNT = 2;
	static SAnimationDef m_animTable[ANIM_TABLE_COUNT];

	EProjectileType m_projectileType = EProjectileType::UNASSIGNED;

	float m_initialPosX = 0.0f;
	float m_initialPosY = 0.0f;
	float m_rotationAngle = 0.0f; // only for Projectiles of type DIAGONAL

	EProjectileOwner m_owner = EProjectileOwner::UNASSIGNED;
};