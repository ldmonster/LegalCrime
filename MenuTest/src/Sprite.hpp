#pragma once

#ifndef Sprite_H
#define Sprite_H

#include <SDL.h>
#include <SDL_Image.h>

#include <string>

#include "../src/Texture.hpp"

class Sprite
{

protected:

	Texture* texture;

	SDL_Rect* clip;

public:

	Sprite(Texture* aTexture, SDL_Rect* clip);
	~Sprite();
	
	SDL_Rect* GetRect();

	void render(SDL_Renderer* renderer, SDL_Rect* renderQuad);

};

#endif

Sprite::Sprite( Texture* aTexture, SDL_Rect* clip )
	: texture { aTexture }
	, clip { clip }
{
}

Sprite::~Sprite()
{
	printf("SPRITE DESTRUCT\n");

	// always delete texture from upper layer
	// because we can reuse textures
	delete clip;
	clip = NULL;

	texture = NULL;
}

SDL_Rect* Sprite::GetRect()
{
	return clip;
}


void Sprite::render(SDL_Renderer* renderer, SDL_Rect* renderQuad)
{
	texture->render(renderer, renderQuad, clip);
}
