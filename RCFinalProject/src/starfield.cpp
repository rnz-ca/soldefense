/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "starfield.h"

#include "app.h"
#include <stdio.h>
#include "utils.h"

void CStarfield::Init()
{
	LOG_SCR_F("Creating Starfield\n");

	// load starfield spritesheet
	m_spriteSheetTexture.CreateFromFile(TEXTURE_SPRITESHEET_FILENAME);

	// set rects of each of the images contained in the spritesheet
	m_nebulaRect = SDL_Rect{ 100, 0, 1920, 1080 };
	m_blueStarRects[static_cast<int>(EStarDistance::DISTANCE_FAR)] = SDL_Rect{ 0, 0, 12, 12 };
	m_blueStarRects[static_cast<int>(EStarDistance::DISTANCE_MEDIUM)] = SDL_Rect{ 0, 12, 25, 25 };
	m_blueStarRects[static_cast<int>(EStarDistance::DISTANCE_CLOSE)] = SDL_Rect{ 0, 37, 50, 50 };
	m_redStarRects[static_cast<int>(EStarDistance::DISTANCE_FAR)] = SDL_Rect{ 0, 87, 12, 12 };
	m_redStarRects[static_cast<int>(EStarDistance::DISTANCE_MEDIUM)] = SDL_Rect{ 0, 99, 25, 25 };
	m_redStarRects[static_cast<int>(EStarDistance::DISTANCE_CLOSE)] = SDL_Rect{ 0, 124, 50, 50 };

	// generate starfield	
	for (int i = 0; i < NUM_STARFIELD_SECTIONS; i++)
	{
		for (int j = 0; j < NUM_STARS; j++)
		{
			m_starsList.AddElement(GenerateStar(CApp::GetInstance()->GetScreenHeight() * (i - 1), CApp::GetInstance()->GetScreenHeight() * i));
		}
	}
}

CStarfield::SStar* CStarfield::GenerateStar(int lowerboundY, int upperboundY)
{
	int starCenterX = Utils::GetRandomUint32(0, CApp::GetInstance()->GetScreenWidth());
	int starCenterY = Utils::GetRandomUint32(lowerboundY, upperboundY);
	int starPosX = starCenterX - static_cast<int>(m_blueStarRects[0].w / 2.0f);
	int starPosY = starCenterY - static_cast<int>(m_blueStarRects[0].h / 2.0f);
	EStarType type = static_cast<EStarType>(Utils::GetRandomUint32(0, static_cast<int>(EStarType::RED)));
	EStarDistance distance = static_cast<EStarDistance>(Utils::GetRandomUint32(0, static_cast<int>(EStarDistance::DISTANCE_CLOSE)));
	return new SStar(SDL_Point{ starPosX, starPosY }, type, distance);
}

void CStarfield::StartAnimation()
{
	m_animationSpeedMultiplier = 1.0f;
	m_animationTicks = Utils::GetTicks();
	m_animationState = EAnimationState::INCREASING_SPEED;
}

void CStarfield::Update(Uint32 elapsedTime)
{
	if (CApp::GetInstance()->GetGameManager()->IsBackgroundScrollingEnabled())
	{
		SStar* starPtr = m_starsList.GetHeadElement();
		while (starPtr != nullptr)
		{
			// determine the speed multiplier while handling the starfield animation
			float speedMultiplier = m_speedMultiplier;
			if (m_animationState != EAnimationState::UNASSIGNED)
			{
				if (Utils::GetTicks() - m_animationTicks > ANIMATION_SPEED_INCREASE_COOLDOWN_MS / m_animationSpeedMultiplier)
				{
					if (m_animationState == EAnimationState::INCREASING_SPEED)
					{
						m_animationSpeedMultiplier += ANIMATION_SPEED_INCREASE;
						if (m_animationSpeedMultiplier > ANIMATION_MAX_SPEED_MULTIPLIER)
						{
							m_animationSpeedMultiplier = ANIMATION_MAX_SPEED_MULTIPLIER;
							m_animationState = EAnimationState::DECREASING_SPEED;
						}
					}
					else // DECREASING_SPEED
					{
						m_animationSpeedMultiplier += ANIMATION_SPEED_DECREASE;
						if (m_animationSpeedMultiplier < ANIMATION_MIN_SPEED_MULTIPLIER)
						{
							m_animationSpeedMultiplier = ANIMATION_MIN_SPEED_MULTIPLIER;
							m_animationState = EAnimationState::UNASSIGNED;
						}
					}					
					m_animationTicks = Utils::GetTicks();
				}
				speedMultiplier = m_animationSpeedMultiplier;
			}

			// update star position
			starPtr->m_position.y += static_cast<int>(Utils::ScaleSpeed(elapsedTime, STAR_SPEED_PER_TYPE_AND_DISTANCE[static_cast<int>(starPtr->m_type)][static_cast<int>(starPtr->m_distance)]) * speedMultiplier);

			// destroy any stars that are off screen
			SDL_Rect* rect = starPtr->m_type == EStarType::BLUE ? m_blueStarRects : m_redStarRects;
			if (starPtr->m_position.y > CApp::GetInstance()->GetScreenHeight() + rect->h / 2.0)
			{
				SStar* nextElement = m_starsList.RemoveElement(starPtr);
				LOG_SCR_F("Deleting star %d\n", (int)(size_t)starPtr);
				delete starPtr;
				starPtr = nextElement;

				m_starsList.AddElement(GenerateStar(CApp::GetInstance()->GetScreenHeight() * -1, 0));
			}
			else
			{				
				starPtr = m_starsList.GetNextElement(starPtr);
			}			
		}
	}

}

void CStarfield::Draw()
{
	// draw the nebula
	m_spriteSheetTexture.Draw(0, 0, &m_nebulaRect);

	if (CApp::GetInstance()->GetGameManager()->IsBackgroundScrollingEnabled())
	{
		// draw the stars
		SStar* starPtr = m_starsList.GetHeadElement();
		while (starPtr != nullptr)
		{
			// do not try drawing any stars outside the screen
			if (starPtr->m_position.y < CApp::GetInstance()->GetScreenHeight())
			{
				SDL_Rect* rect = starPtr->m_type == EStarType::BLUE ? m_blueStarRects : m_redStarRects;
				m_spriteSheetTexture.Draw(starPtr->m_position.x, starPtr->m_position.y, &rect[static_cast<int>(starPtr->m_distance)]);
			}

			starPtr = m_starsList.GetNextElement(starPtr);
		}
	}	
}

void CStarfield::Destroy()
{
	LOG_SCR_F("Destroying Starfield\n");
	m_spriteSheetTexture.Destroy();
}