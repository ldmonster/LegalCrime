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

	SDL_Rect clip;

	//Uint8 alpha{ 255 };

public:

	Sprite(Texture* aTexture, SDL_Rect clip);
	~Sprite();

	void render(SDL_Renderer* renderer, Uint16 x, Uint16 y, SDL_Rect* renderQuad);

};

#endif

Sprite::Sprite( Texture* aTexture, SDL_Rect clip )
	: texture { aTexture }
	, clip { clip }
{
}

Sprite::~Sprite()
{
	texture = nullptr;
}

void Sprite::render(SDL_Renderer* renderer, Uint16 x, Uint16 y, SDL_Rect* renderQuad)
{
	SDL_RenderCopy(renderer, texture->getTexture(), &clip, renderQuad);
}
