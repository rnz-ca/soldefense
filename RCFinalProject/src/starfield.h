#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "doublelinkedlist.h"
#include "texture.h"

class CStarfield
{
public:
	void Init();
	void Update(Uint32 elapsedTime);
	void Draw();
	void Destroy();

	void SetSpeedMultiplier(float multiplier) { m_speedMultiplier = multiplier; }

	void StartAnimation();
	bool IsPlayingAnimation() { return m_animationState != EAnimationState::UNASSIGNED; }

private:
	// textures
	const std::string TEXTURE_SPRITESHEET_FILENAME = "starfield_spritesheet.png";

	enum class EStarDistance : int
	{
		DISTANCE_FAR = 0,
		DISTANCE_MEDIUM = 1,
		DISTANCE_CLOSE = 2
	};

	enum class EStarType : int
	{
		BLUE,
		RED
	};

	struct SStarInfo
	{
		SDL_Point	m_position;
		int			m_screenYPos;
		EStarType	m_starType;
	};

	struct SStar
	{
		SDL_Point		m_position;
		EStarType		m_type;
		EStarDistance	m_distance;

		SStar(SDL_Point position, EStarType type, EStarDistance distance)
		{
			m_position = position;
			m_type = type;
			m_distance = distance;
		}
	};

	// per section - starfield is composed of two section, each the same size as the screen to
	// create the illusion of scrolling
	const int NUM_STARS = 50;
	
	const int NUM_STARFIELD_SECTIONS = 2;
	static const int NUM_STAR_PER_DISTANCE = 20;
	static const int NUM_STAR_DISTANCES = 3;
	static const int NUM_STAR_TYPES = 2;
	const float STAR_SPEED_PER_TYPE_AND_DISTANCE[NUM_STAR_TYPES][NUM_STAR_DISTANCES] = {
		{
			// BLUE STARS
			15.0f, // FAR
			30.0f, // MEDIUM
			45.0f  // CLOSE
		},
		{
			// RED STARS
			17.0f, // FAR
			32.0f, // MEDIUM
			47.0f  // CLOSE
		}
		
	};
	
	enum class EAnimationState : int
	{
		UNASSIGNED,
		INCREASING_SPEED,
		DECREASING_SPEED
	};

	const float ANIMATION_SPEED_INCREASE = 0.1f;
	const float ANIMATION_SPEED_DECREASE = ANIMATION_SPEED_INCREASE * -1.0f;
	const float ANIMATION_SPEED_INCREASE_COOLDOWN_MS = 20;
	const float ANIMATION_MIN_SPEED_MULTIPLIER = 1.0f;
	const float ANIMATION_MAX_SPEED_MULTIPLIER = 12.0f;
	
	CStarfield::SStar* GenerateStar(int lowerboundY, int upperboundY);
	
	CDoubleLinkedList<SStar*> m_starsList;
	CTexture m_spriteSheetTexture;
	SDL_Rect m_nebulaRect;
	SDL_Rect m_blueStarRects[NUM_STAR_DISTANCES] = {};
	SDL_Rect m_redStarRects[NUM_STAR_DISTANCES] = {};
	SStarInfo m_stars[NUM_STAR_DISTANCES][NUM_STAR_PER_DISTANCE];
	float m_speedMultiplier = 1.0f;

	float m_animationSpeedMultiplier = 0.0f;
	Uint32 m_animationTicks = 0;
	EAnimationState m_animationState = EAnimationState::UNASSIGNED;

};