#pragma once

#ifndef Texture_H
#define Texture_H

#include <SDL.h>
#include <SDL_Image.h>
#include <SDL_ttf.h>

#include <string>

class Texture
{

protected:

	SDL_Texture* m_sdlTexture;

	Uint16 width;
	Uint16 height;

	TTF_Font* m_font;
	
public:

	Texture(SDL_Texture* sdlTexture = nullptr);
	~Texture();

	bool loadFromFile(SDL_Renderer* renderer, std::string path);
	bool loadFromRenderedText(SDL_Renderer* renderer, std::string textureText, SDL_Color textColor);
	
	bool SetFont(std::string fontPath, Uint8 ptSize);

	void setColor(Uint8 red, Uint8 green, Uint8 blue);
	void setBlendMode(SDL_BlendMode blending);
	void setAlpha(Uint8 alpha);

	//void render(SDL_Renderer* renderer, SDL_Rect* clip, SDL_Rect* renderQuad);
	void render(SDL_Renderer* renderer, SDL_Rect* renderQuad, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	int getWidth();
	int getHeight();
	SDL_Texture* getTexture();

};

#endif

Texture::Texture(SDL_Texture* sdlTexture)
	: width { 0 }
	, height { 0 }
	, m_font { nullptr }
{
	m_sdlTexture = sdlTexture;
}

Texture::~Texture()
{
	printf("TEXTURE DESTRUCT\n");

	SDL_DestroyTexture(m_sdlTexture);
	m_sdlTexture = NULL;

	width = 0;
	height = 0;
}

int Texture::getWidth()
{
	return width;
}

int Texture::getHeight()
{
	return height;
}

SDL_Texture* Texture::getTexture()
{
	return m_sdlTexture;
}

bool Texture::SetFont(std::string fontPath, Uint8 ptSize) {
	m_font = TTF_OpenFont(fontPath.c_str(), ptSize);
	if (m_font == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());

		return false;
	}

	//TTF_SetFontStyle(m_font, TTF_STYLE_BOLD);

	return true;
}


bool Texture::loadFromFile(SDL_Renderer* renderer, std::string path)
{
	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());

		return false;
	}

	newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
	if (newTexture == NULL)
	{
		printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
			
		return false;
	}

	width = loadedSurface->w;
	height = loadedSurface->h;

	SDL_FreeSurface(loadedSurface);

	m_sdlTexture = newTexture;

	return true;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool Texture::loadFromRenderedText(SDL_Renderer* renderer, std::string textureText, SDL_Color textColor)
{
	if (m_sdlTexture != NULL)
	{
		SDL_DestroyTexture(m_sdlTexture);
	}

	SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, textureText.c_str(), textColor);
	if (textSurface != NULL)
	{
		m_sdlTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (m_sdlTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());

			return false;
		}
		else
		{
			width = textSurface->w;
			height = textSurface->h;
		}

		SDL_FreeSurface(textSurface);
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());

		return false;
	}

	return true;
}
#endif

void Texture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_SetTextureColorMod(m_sdlTexture, red, green, blue);
}

void Texture::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(m_sdlTexture, blending);
}

void Texture::setAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(m_sdlTexture, alpha);
}

//void Texture::render(SDL_Renderer* renderer, SDL_Rect* renderQuad, SDL_Rect* clip)
//{
//	SDL_RenderCopy(renderer, m_sdlTexture, clip, renderQuad);
//}

void Texture::render(SDL_Renderer* renderer, SDL_Rect* renderQuad, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_RenderCopyEx(renderer, m_sdlTexture, clip, renderQuad, angle, center, flip);
}