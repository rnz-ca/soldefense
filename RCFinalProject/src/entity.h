#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#if __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#include "animationmanager.h"
#include "texture.h"

class CEntity
{
public:	
	enum EEntityType
	{
		UNASSIGNED,
		PLAYERSHIP,
		ENEMY,
		PROJECTILE,
		BOSS,
		EXPLOSION
	};

	EEntityType GetType() { return m_type; }

	virtual void Init(CTexture* spriteSheetTexture);

	virtual void SetSpriteSheetTexture(CTexture* spriteSheetTexture);

	virtual void SetPosition(float x, float y);
	virtual void MoveX(float x);
	virtual void MoveY(float y);
	virtual void Update(Uint32 elapsedTime);
	virtual void Draw() = 0;

	virtual void OnCollision() {};

	void SetSpriteWidth(int w) { m_spriteWidth = w; }
	int GetSpriteWidth() { return m_spriteWidth; }
	void SetSpriteHeight(int h) { m_spriteHeight = h; }
	int GetSpriteHeight() { return m_spriteHeight; }
	void SetSpriteDimensions(int w, int h)
	{
		SetSpriteWidth(w);
		SetSpriteHeight(h);
	}
	
	void SetMoveSpeedX(float msx) { m_moveSpeedX = msx; }
	float GetMoveSpeedX() { return m_moveSpeedX; }
	void SetMoveSpeedY(float msy) { m_moveSpeedY = msy; }
	float GetMoveSpeedY() { return m_moveSpeedY; }

	float GetPosX() { return m_x; }
	float GetPosY() { return m_y; }
	float GetCenterPointX() { return m_x + m_spriteWidth / 2.0f; }
	float GetCenterPointY() { return m_y + m_spriteHeight / 2.0f; }

	SDL_Rect GetRect() { return SDL_Rect{static_cast<int>(m_x), static_cast<int>(m_y), GetSpriteWidth(), GetSpriteHeight()}; }
	SDL_Rect GetColliderRect();

	float GetColliderScaleFactor() { return m_colliderScaleFactor; }
	void SetColliderScaleFactor(float colliderScaleFactor) { m_colliderScaleFactor = colliderScaleFactor; }

	bool CollidesWith(CEntity* entity);

	virtual bool IsAlive() { return m_isAlive; }
	void SetIsAlive(bool isAlive) { m_isAlive = isAlive; }

protected:
	EEntityType m_type = EEntityType::UNASSIGNED;

	float m_x = 0;
	float m_y = 0;
	float m_speed_x = 0.0f;
	float m_speed_y = 0.0f;

	CAnimationManager m_animationMgr;

	CTexture* m_spriteSheetTexture = nullptr;
	int m_spriteWidth = 0;
	int m_spriteHeight = 0;
	float m_colliderScaleFactor = 1.0f;

	float m_moveSpeedX = 0;
	float m_moveSpeedY = 0;

	bool m_isAlive = true;
};