/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "entity.h"

#include <assert.h>
#include "utils.h"

void CEntity::Init(CTexture* spriteSheetTexture)
{
	SetSpriteSheetTexture(spriteSheetTexture);
}

void CEntity::SetSpriteSheetTexture(CTexture* spriteSheetTexture)
{
	m_spriteSheetTexture = spriteSheetTexture;
}

void CEntity::SetPosition(float x, float y)
{
	m_x = x;
	m_y = y;
}

void CEntity::MoveX(float x)
{
	m_x = x;
}

void CEntity::MoveY(float y)
{
	m_y = y;
}

void CEntity::Update(Uint32 elapsedTime)
{
}

SDL_Rect CEntity::GetColliderRect()
{
	SDL_Rect rect = GetRect();

	// optimization to only scale the rect if the collider scale factor is different than 1.0f
	if (m_colliderScaleFactor != 1.0f)
	{
		// calculate midpoint of original rect
		int midPointX = rect.x + rect.w;
		int midPointY = rect.y + rect.h;

		// scale dimensions
		int scaledWidth = static_cast<int>(rect.w * m_colliderScaleFactor);
		int scaledHeight = static_cast<int>(rect.h * m_colliderScaleFactor);

		// calculate x/y based on scaled dimensions
		int newX = midPointX - scaledWidth;
		int newY = midPointY - scaledHeight;

		rect.x = newX;
		rect.y = newY;
		rect.w = scaledWidth;
		rect.h = scaledHeight;
	}

	return rect;
}

bool CEntity::CollidesWith(CEntity* entityPtr)
{
	assert(entityPtr != nullptr);
	return Utils::CheckRectIntersection(GetColliderRect(), entityPtr->GetColliderRect());
}