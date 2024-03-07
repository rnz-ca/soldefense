#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#if __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#include <string>

// wrapper class for an SDL Texture object
class CTexture
{
public:
	enum class EFont : int
	{
		REGULAR,
		BIG
	};

	bool CreateFromFile(const std::string& filename);
	bool CreateFromText(const std::string& text, SDL_Color color, EFont font = EFont::REGULAR);

	void SetBlendMode(SDL_BlendMode mode);
	void GetTint(Uint8* r, Uint8* g, Uint8* b);
	void SetTint(Uint8 r, Uint8 g, Uint8 b);
	void SetAlpha(Uint8 a);

	void Draw(int x, int y, SDL_Rect* sourceRect = nullptr, double angleInDegrees = 0.0, SDL_Point* rotationCenterPoint = nullptr, SDL_RendererFlip textureFlipping = SDL_FLIP_NONE);

	void Destroy();

	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }
	bool IsCreated() { return m_texture != nullptr; }

private:
	SDL_Texture* m_texture = nullptr;
	int m_width = 0;
	int m_height = 0;
};