/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "projectile.h"

#include "ingamestate.h"
#include "utils.h"

//  Anim Table
SAnimationDef CProjectile::m_animTable[CProjectile::ANIM_TABLE_COUNT] =
{
	SAnimationDef(0, 0, 4, 50, SAnimationDef::EPlayMethod::LOOP),
	SAnimationDef(0, 185, 16, 50, SAnimationDef::EPlayMethod::LOOP)
};

CProjectile::CProjectile()
{
	m_type = EEntityType::PROJECTILE;
}

void CProjectile::Init(EProjectileOwner owner, EProjectileType projectileType, CTexture* spriteSheetTexture, float x, float y)
{
	CEntity::Init(spriteSheetTexture);

	m_owner = owner;
	m_projectileType = projectileType;

	CIngameState* ingameState = dynamic_cast<CIngameState*>(CApp::GetInstance()->GetGameManager()->GetState());

	int spriteWidth = 0;
	int spriteHeight = 0;
	EAnimID animId = EAnimID::PLAYER_IDLE;
	float moveSpeedX = 0.0f;
	float moveSpeedY = 0.0f;
	int spawnYOffset = 0;

	if (m_owner == EProjectileOwner::PLAYER)
	{
		spriteWidth = PLAYER_PROJECTILE_SPRITE_WIDTH;
		spriteHeight = PLAYER_PROJECTILE_SPRITE_HEIGHT;		
		animId = EAnimID::PLAYER_IDLE;
		moveSpeedX = PLAYER_PROJECTILE_MOVE_SPEED_X;
		moveSpeedY = PLAYER_PROJECTILE_MOVE_SPEED_Y;
		spawnYOffset = PLAYER_PROJECTILE_SPAWN_Y_OFFSET;
	}
	else if (m_owner == EProjectileOwner::ENEMY)
	{
		spriteWidth = ENEMY_PROJECTILE_SPRITE_WIDTH;
		spriteHeight = ENEMY_PROJECTILE_SPRITE_HEIGHT;
		animId = EAnimID::ENEMY_IDLE;
		if (projectileType == EProjectileType::DIAGONAL)
		{
			// calculate the angle between the initial position and the player ship
			float playerShipX = ingameState->GetPlayerShipPosX();
			float playerShipY = ingameState->GetPlayerShipPosY();

			// use arc tangent to determine the angle in radians
			double angle = atan2(playerShipY - y, playerShipX - x);

			// convert the angle from radians to degrees
			m_rotationAngle = -static_cast<float>(angle * (180.0 / M_PI));

			// calculate the moveSpeed depending on the angle
			moveSpeedX = static_cast<float>(cos(angle) * ENEMY_PROJECTILE_MOVE_SPEED);
			moveSpeedY = static_cast<float>(sin(angle) * ENEMY_PROJECTILE_MOVE_SPEED);
		}
		else // REGULAR
		{
			moveSpeedX = ENEMY_PROJECTILE_MOVE_SPEED;
			moveSpeedY = ENEMY_PROJECTILE_MOVE_SPEED;
		}
		
		spawnYOffset = ENEMY_PROJECTILE_SPAWN_Y_OFFSET;
	}

	SetSpriteWidth(spriteWidth);
	SetSpriteHeight(spriteHeight);
	SetMoveSpeedX(moveSpeedX);
	SetMoveSpeedY(moveSpeedY);

	// set initial position
	m_initialPosX = x;
	m_initialPosY = y;
	SetPosition(x, y);

	// initialize animation
	m_animationMgr.Setup(m_animTable, spriteWidth, spriteHeight, spriteSheetTexture);
	m_animationMgr.RequestAnimation(static_cast<int>(animId));

	SetIsAlive(true);
}

void CProjectile::Update(Uint32 elapsedTime)
{
	if (IsAlive())
	{
		if (m_projectileType == EProjectileType::REGULAR)
		{
			float moveY = Utils::ScaleSpeed(elapsedTime, m_moveSpeedY);
			m_y += moveY;

			
		}
		else if (m_projectileType == EProjectileType::DIAGONAL)
		{
			float moveX = Utils::ScaleSpeed(elapsedTime, m_moveSpeedX);
			float moveY = Utils::ScaleSpeed(elapsedTime, m_moveSpeedY);
			m_x += moveX;
			m_y += moveY;
		}

		if (m_y < 0.0f || m_y > CApp::GetInstance()->GetScreenHeight() ||
			m_x < 0.0f || m_x > CApp::GetInstance()->GetScreenWidth())
		{
			SetIsAlive(false);
		}

		m_animationMgr.Update(elapsedTime);
	}
}

void CProjectile::Draw()
{
	m_animationMgr.Draw(static_cast<int>(m_x), static_cast<int>(m_y), m_rotationAngle);
}

void CProjectile::OnCollision()
{
	SetIsAlive(false);
}