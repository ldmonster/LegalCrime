#pragma once

#ifndef Menu_H
#define Menu_H

#include <SDL.h>

#include <vector>
#include <string>

#include "../src/Texture.hpp"
#include "../src/Sprite.hpp"

class MenuPage
{

protected:

	SDL_Renderer* renderer;
	Sprite* background;

public:

	MenuPage();
	~MenuPage();

	virtual bool init() = 0;
	virtual bool render() = 0;
	virtual void handleEvent(SDL_Event* e) = 0;


};

class MainPage : public MenuPage
{
	enum MainPageButtons {
		SingleplayerButton,
		MultiplayerButton,
		SettingsButton,
		CreditsButton,
		CultureButton,
		ExitButton,
		OverallButtons
	};

protected:

	SDL_Renderer* renderer;

	Sprite* background;
	Sprite* gSpriteClips[OverallButtons];
	bool pageIsActive[OverallButtons];

public:

	MainPage(SDL_Renderer* renderer);
	~MainPage();

	bool init();
	bool render();
	void handleEvent(SDL_Event* e);

};

#endif

MenuPage::MenuPage()
	: renderer{ renderer }
	, background{ nullptr }
{

}

MenuPage::~MenuPage()
{

}

MainPage::MainPage(SDL_Renderer* renderer)
	: renderer {renderer}
	, background { nullptr }
{
	for ( int i = 0; i < OverallButtons; i++ ) {
		gSpriteClips[i] = nullptr;
	}
}

MainPage::~MainPage()
{
	renderer = nullptr;

	background = NULL;

	for (int i = 0; i < OverallButtons; i++) {
		gSpriteClips[i] = NULL;
	}
}

bool MainPage::init()
{
	Texture* bgTexture = new Texture();
	bgTexture->loadFromFile(renderer, "./Pics/main_menu.png");

	SDL_Rect clip = SDL_Rect{0,0,bgTexture->getWidth(),bgTexture->getHeight()};
	background = new Sprite(bgTexture, clip);

	Texture* spTexture = new Texture();
	spTexture->loadFromFile(renderer, "./Pics/main_menu_buttons.png");
	int xButton = 0;
	int yButton = 0;
	int widthButton = 566;
	int heightButton = 84;

	for (int i = 0; i < OverallButtons; i++)
	{
		SDL_Rect clip = SDL_Rect{ xButton, yButton, widthButton, heightButton };
		gSpriteClips[i] = new Sprite(spTexture, clip);

		yButton += heightButton;
	}

	return true;
}

bool MainPage::render()
{
	SDL_Rect* renderQuad = new SDL_Rect{ 0, 0, 800, 600};
	background->render(renderer, 0, 0, renderQuad);

	int yButton = 152;
	int heightButton = 42;
	int offsetButton = 26;

	for (int i = 0; i < OverallButtons; i++)
	{
		if (pageIsActive[i] == true) {
			SDL_Rect* renderQuad = new SDL_Rect{ 257, yButton, 283, heightButton };
			gSpriteClips[i]->render(renderer, 0, 0, renderQuad);
		}

		yButton += heightButton + offsetButton;
	}

	return true;
}

void MainPage::handleEvent(SDL_Event* e)
{
	int xButton = 257;
	int yButton = 152;
	int widthButton = 283;
	int heightButton = 42;
	int offsetButton = 26;

	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		for (int i = 0; i < OverallButtons; i++)
		{
			pageIsActive[i] = false;
		}

		int x, y;
		SDL_GetMouseState(&x, &y);

		for (int i = 0; i < OverallButtons; i++)
		{
			if (x > xButton && x < xButton + widthButton && y > yButton && y < yButton + heightButton)
			{
				pageIsActive[i] = true;

				return;
			}

			yButton += heightButton + offsetButton;
		}
	}
}