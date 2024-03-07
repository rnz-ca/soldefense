/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "utils.h"

#if __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#include <cstdlib>
#include "preproc.h"
#include <time.h>

namespace Utils
{
	float ScaleSpeed(Uint32 elapsedTime, float speed)
	{
		return (static_cast<float>(elapsedTime) / SCALE_FACTOR) * speed;
	}

	Uint32 GetTicks()
	{
		return SDL_GetTicks();
	}

	bool CheckRectIntersection(SDL_Rect r1, SDL_Rect r2)
	{
		int r1_x1 = r1.x;
		int r1_y1 = r1.y;
		int r1_x2 = r1.x + r1.w;
		int r1_y2 = r1.y + r1.h;

		int r2_x1 = r2.x;
		int r2_y1 = r2.y;
		int r2_x2 = r2.x + r2.w;
		int r2_y2 = r2.y + r2.h;

		return !(r1_y2 <= r2_y1 || r1_y1 >= r2_y2 || r1_x2 <= r2_x1 || r1_x1 >= r2_x2);
	}

	void RandomizeRngSeed()
	{
		srand(static_cast<unsigned int>(time(nullptr)));
	}

	uint32_t GetRandomUint32(uint32_t min, uint32_t max)
	{
		return min + std::rand() % (max + 1 - min);
	}
}
