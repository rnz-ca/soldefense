/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "texture.h"

#include "app.h"
#include <assert.h>
#if __APPLE__
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#else
#include <SDL_Image.h>
#include <SDL_ttf.h>
#endif
#include <stdio.h>
#include "utils.h"

bool CTexture::CreateFromFile(const std::string& filename)
{
	// if the texture is already initialized, destroy it first
	if (m_texture != nullptr)
	{
		Destroy();
	}

	std::string path = GFX_DIRECTORY;
	path.append(filename);

	SDL_LogMessage(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_INFO, "Loading texture: %s", path.c_str());
	m_texture = IMG_LoadTexture(CApp::GetInstance()->GetRenderer(), path.c_str());
	if (m_texture == nullptr)
	{
		LOG_SCR_F("Unable to load texture: %s (%s)\n", path.c_str(), SDL_GetError());
		return false;
	}
	
	if (SDL_QueryTexture(m_texture, nullptr, nullptr, &m_width, &m_height) < 0)
	{
		Destroy();
		LOG_SCR_F("Texture loaded successfully: %s (%d)\n", path.c_str(), (int)(size_t)m_texture);
		return false;
	}

	LOG_SCR_F("Texture loaded successfully: %s (%d)\n", path.c_str(), (int)(size_t)m_texture);
	return true;
}

bool CTexture::CreateFromText(const std::string& text, SDL_Color color, EFont font)
{
	// if the texture is already initialized, destroy it first, mostly used when updating text in a text texture
	if (m_texture != nullptr)
	{
		Destroy();
	}

	TTF_Font* fontPtr = font == EFont::REGULAR ? CApp::GetInstance()->GetRegularFont() : CApp::GetInstance()->GetBigFont();

	SDL_Surface* surf = TTF_RenderText_Solid(fontPtr, text.c_str(), color);
	if (surf == nullptr)
	{
		LOG_SCR_F("Unable to render text: %s (%s)\n", text.c_str(), TTF_GetError());
		return false;
	}

	// create the texture from the SDL surface
	m_texture = SDL_CreateTextureFromSurface(CApp::GetInstance()->GetRenderer(), surf);
	if (m_texture == nullptr)
	{
		SDL_FreeSurface(surf); // free the surface
		LOG_SCR_F("Unable to create texture from surface: %s (%s)\n", text.c_str(), SDL_GetError());
		return false;
	}

	// store the dimensions
	m_width = surf->w;
	m_height = surf->h;

	// free the surface
	SDL_FreeSurface(surf); 

	return true;
}

// useful for transparency effects
void CTexture::SetBlendMode(SDL_BlendMode mode)
{
	SDL_SetTextureBlendMode(m_texture, mode);
}

// fetch current tint of the texture
void CTexture::GetTint(Uint8* r, Uint8* g, Uint8* b)
{
	SDL_GetTextureColorMod(m_texture, r, g, b);
}

// to tint images for color blind/accessibility options
void CTexture::SetTint(Uint8 r, Uint8 g, Uint8 b)
{
	SDL_SetTextureColorMod(m_texture, r, g, b);
}

// texture transparency
void CTexture::SetAlpha(Uint8 a)
{
	SDL_SetTextureAlphaMod(m_texture, a);
}

// main drawing function
void CTexture::Draw(int x, int y, SDL_Rect* sourceRect, double angleInDegrees, SDL_Point* rotationCenterPoint, SDL_RendererFlip textureFlipping)
{
	assert(m_texture != nullptr);

	int w = m_width;
	int h = m_height;

	// source rect is optional, but used for clipping an image if needed
	if (sourceRect != nullptr)
	{
		w = sourceRect->w;
		h = sourceRect->h;
	}
	SDL_Rect dstRect{ x, y, w, h };
	
	// call the main SDL render function that permits rotation and flipping as well if needed
	SDL_RenderCopyEx(CApp::GetInstance()->GetRenderer(), m_texture, sourceRect, &dstRect, angleInDegrees, rotationCenterPoint, textureFlipping);
}

void CTexture::Destroy()
{
	if (m_texture != nullptr)
	{
		SDL_DestroyTexture(m_texture);

		// resetting all values to their initial state
		m_texture = nullptr;
		m_width = 0;
		m_height = 0;
	}
}