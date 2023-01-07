#pragma once

#ifndef Texture_H
#define Texture_H

#include <SDL.h>
#include <SDL_Image.h>

#include <string>

class Texture
{

protected:

	SDL_Texture* sdlTexture;

	Uint16 width;
	Uint16 height;
	
public:

	Texture();
	~Texture();

	bool loadFromFile(SDL_Renderer* renderer, std::string path);

	void render(SDL_Renderer* renderer, SDL_Rect* clip, SDL_Rect* renderQuad);

	int getWidth();
	int getHeight();
	SDL_Texture* getTexture();

};

#endif

Texture::Texture()
	: sdlTexture{ nullptr }
	, width { 0 }
	, height { 0 }
{
}

Texture::~Texture()
{
	printf("TEXTURE DESTRUCT\n");

	SDL_DestroyTexture(sdlTexture);
	sdlTexture = NULL;

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
	return sdlTexture;
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

	sdlTexture = newTexture;

	return true;
}

void Texture::render(SDL_Renderer* renderer, SDL_Rect* clip, SDL_Rect* renderQuad)
{
	SDL_RenderCopy(renderer, sdlTexture, clip, renderQuad);
}