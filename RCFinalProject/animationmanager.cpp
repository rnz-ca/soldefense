/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "animationmanager.h"

#include <assert.h>
#include "utils.h"

void CAnimationManager::Setup(const SAnimationDef* animTable, int frameWidth, int frameHeight)
{
	m_animTable = animTable;
	m_frameWidth = frameWidth;
	m_frameHeight = frameHeight; 
	VerifyInitialization();
}

void CAnimationManager::Setup(const SAnimationDef* animTable, int frameWidth, int frameHeight, CTexture* spriteSheetTexture)
{
	Setup(animTable, frameWidth, frameHeight);
	m_spriteSheetTexture = spriteSheetTexture;
	VerifyInitialization();
}

void CAnimationManager::VerifyInitialization()
{
	m_isInitialized = m_animTable != nullptr && m_frameWidth > 0 && m_frameHeight > 0 && m_spriteSheetTexture != nullptr;
}

void CAnimationManager::RequestAnimation(int index)
{
	m_requestedAnimIndex = index;
}

void CAnimationManager::Update(Uint32 elapsedTime)
{
	assert(m_isInitialized);

	if (m_requestedAnimIndex != ANIMATION_NONE)
	{
		// there is a request for an animation
		m_ticks = Utils::GetTicks();
		m_currentAnimIndex = m_requestedAnimIndex;

		// reset the current frame variable
		m_currentFrame = 0;

		// clear the requested animation index variable
		m_requestedAnimIndex = ANIMATION_NONE;
	}

	// check if it's time to update the frame
	if (m_currentAnimIndex != ANIMATION_NONE)
	{
		const SAnimationDef& animDef = m_animTable[m_currentAnimIndex];
		if (Utils::GetTicks() - m_ticks > animDef.m_time)
		{
			// update the frame
			m_currentFrame += m_playDirection;
			
			// wrap around if we reached the maximum number of frames
			if (m_currentFrame < 0 || m_currentFrame >= animDef.m_numFrames)
			{
				if (animDef.m_playMethod == SAnimationDef::EPlayMethod::LOOP) // if anim has to be looped...
				{
					m_currentFrame = 0;
				}
				else if (animDef.m_playMethod == SAnimationDef::EPlayMethod::LOOP_BOOMERANG)
				{
					if (m_currentFrame < 0)
					{
						m_currentFrame = 1;
					}
					else
					{
						m_currentFrame = animDef.m_numFrames - 1;
					}
					m_playDirection = m_playDirection * -1; // invert the direction
				}
				else
				{
					m_currentFrame = animDef.m_numFrames - 1;
				}				
			}

			m_ticks = Utils::GetTicks();
		}
	}	
}

void CAnimationManager::Draw(int posX, int posY, double angleInDegrees)
{
	assert(m_isInitialized);

	if (m_currentAnimIndex != ANIMATION_NONE)
	{
		const SAnimationDef& animDef = m_animTable[m_currentAnimIndex];

		// calculate the position of the image in the spritesheet based on current frame index
		int ssPosX = animDef.m_startPosX + m_frameWidth * m_currentFrame;

		// blit the right frame of the current anim
		SDL_Rect srcRect{ ssPosX, animDef.m_startPosY, m_frameWidth, m_frameHeight };
		m_spriteSheetTexture->Draw(posX, posY, &srcRect, angleInDegrees);
	}
}