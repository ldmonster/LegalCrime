#pragma once

#ifndef Button_H
#define Button_H

#include <SDL.h>
#include <SDL_mixer.h>

#include <string>

#include "../src/Texture.hpp"
#include "../src/Sprite.hpp"

class Button
{

protected:

	Sprite* spriteOver;
	Sprite* spriteHit;

	SDL_Rect* renderQuad;

	Mix_Chunk* overButtonSound;
	Mix_Chunk* hitButtonSound;

	bool mouseOver;
	bool mouseHit;

	bool mouseOverRendered;
	bool mouseHitRendered;

public:

	Button(Sprite* aSpriteOver, Sprite* aSpriteHit, SDL_Rect* aRenderQuad, Mix_Chunk* aOverButtonSound, Mix_Chunk* aHitButtonSound);
	~Button();

	bool isOver();
	bool isHit();
	bool isOverRendered();
	bool isHitRendered();

	void over();
	void hit();

	void renderOver(SDL_Renderer* renderer);
	void renderHit(SDL_Renderer* renderer);

	void playOverSound();
	void playHitSound();

	void resetButtonStates();

};

#endif


Button::Button(Sprite* aSpriteOver, Sprite* aSpriteHit, SDL_Rect* aRenderQuad, Mix_Chunk* aOverButtonSound, Mix_Chunk* aHitButtonSound)
	: spriteOver{ aSpriteOver }
	, spriteHit { aSpriteHit }
	, renderQuad{ aRenderQuad }
	, overButtonSound { aOverButtonSound }
	, hitButtonSound { aHitButtonSound }
	, mouseOver { false }
	, mouseHit { false }
	, mouseOverRendered { false }
	, mouseHitRendered { false }
{
}

Button::~Button()
{
	printf("BUTTON DESTRUCT\n");
	
	delete renderQuad;
	delete spriteOver;
	delete spriteHit;
	renderQuad = NULL;
	spriteOver = NULL;
	spriteHit = NULL;
}

bool Button::isOver()
{
	return mouseOver;
}

bool Button::isHit()
{
	return mouseHit;
}

bool Button::isOverRendered()
{
	return mouseOverRendered;
}

bool Button::isHitRendered()
{
	return mouseHitRendered;
}

void Button::over()
{
	mouseOver = true;
}

void Button::hit()
{
	mouseHit = true;
}

void Button::renderOver(SDL_Renderer* renderer)
{
	spriteOver->render(renderer, renderQuad);
	mouseOverRendered = true;
}

void Button::playOverSound()
{
	Mix_PlayChannel(-1, overButtonSound, 0);
}

void Button::renderHit(SDL_Renderer* renderer)
{
	spriteHit->render(renderer, renderQuad);
	mouseHitRendered = true;
}

void Button::playHitSound()
{
	Mix_PlayChannel(-1, hitButtonSound, 0);
}

void Button::resetButtonStates()
{
	mouseOver = false;
	mouseHit = false;
	mouseOverRendered = false;
	mouseHitRendered = false;
}