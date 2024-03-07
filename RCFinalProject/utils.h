#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#if __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#include <stdio.h>

#define DELETE_OBJ(x)		delete x; x = nullptr;
#if _DEBUG
#define LOG_SCR(x)			printf(x "\n")
#define LOG_SCR_F(...)		printf(__VA_ARGS__)
#else
#define LOG_SCR(x)			;
#define LOG_SCR_F(...)		;
#endif

namespace Utils
{
	struct SGridLocation8
	{
		Uint8 m_row = 0;
		Uint8 m_column = 0;
	};

	float ScaleSpeed(Uint32 elapsedTime, float speed);
	bool CheckRectIntersection(SDL_Rect r1, SDL_Rect r2);
	Uint32 GetTicks();
	void RandomizeRngSeed();
	uint32_t GetRandomUint32(uint32_t min, uint32_t max);	
}
