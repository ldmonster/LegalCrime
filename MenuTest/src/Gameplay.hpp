#pragma once

#ifndef Gameplay_H
#define Gameplay_H

#include <SDL.h>

#include <string>

#include "../src/Game.hpp"
#include "../src/Map.hpp"
#include "../src/Texture.hpp"

class Gameplay: public GameSection
{

protected:

	SDL_Renderer* m_renderer;

	Map* m_map;

	bool m_isInitialized;

	std::string m_lastError;

public:

	Gameplay(SDL_Renderer* m_renderer);
	~Gameplay();

	bool init();
	bool render();

	void handleEvent(SDL_Event* e);

	bool isInitialized();

	GameSections GetGameSection();

	std::string GetLastError();

};

#endif

Gameplay::Gameplay(SDL_Renderer* renderer)
{
	m_renderer = renderer;

	m_map = new Map(10, 10);
	m_map->init();

	m_isInitialized = false;
}

Gameplay::~Gameplay()
{

}

bool Gameplay::init()
{
	m_isInitialized = true;

	return true;
}

bool Gameplay::render()
{
	m_map->render(m_renderer);

	return true;
}


void Gameplay::handleEvent(SDL_Event* e)
{
	m_map->handleEvent(e);
}


bool Gameplay::isInitialized()
{
	return m_isInitialized;
}

GameSections Gameplay::GetGameSection() 
{
	return PlayingGameSection;
}

std::string Gameplay::GetLastError()
{
	return m_lastError;
}
