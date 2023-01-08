#pragma once

#ifndef Menu_H
#define Menu_H

#include <SDL.h>
#include <SDL_mixer.h>

#include <vector>
#include <string>

#include "../src/helpers/StringsHelper.hpp"

#include "../Sound.hpp"
#include "../src/Texture.hpp"
#include "../src/Sprite.hpp"
#include "../src/Button.hpp"

class MenuPage : public GameSection
{

protected:

public:

	virtual ~MenuPage() {};

	virtual bool init() = 0;
	virtual bool render() = 0;
	virtual void handleEvent(SDL_Event* e) = 0;

	virtual bool isInitialized() = 0;
	virtual bool isShutdown() = 0;

	virtual std::string GetLastError() = 0;

};

class MainPage : public MenuPage
{
	enum MainPageTextures {
		BackgroundTexture,
		BackgroundAssestTexture,
		OverallTextures
	};

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

	MainMenu_Sound* sound;

	Texture* textures[OverallTextures];

	Sprite* background;
	SDL_Rect* backgroundRenderQuad;

	Button* buttons[OverallButtons];
	Uint8 buttonStateChanged;

	bool initialized;
	bool shutdown;

	std::string lastError;

public:

	MainPage(SDL_Renderer* renderer);
	~MainPage();

	bool init();
	bool render();
	void handleEvent(SDL_Event* e);
	
	bool isInitialized();
	bool isShutdown();

	std::string GetLastError();

};

#endif

MainPage::MainPage(SDL_Renderer* renderer)
	: renderer {renderer}
	, background{ nullptr }
	, backgroundRenderQuad{ nullptr }
	, initialized{ false }
	, shutdown{ false }
	, buttonStateChanged{ 5 }
	, sound { nullptr }
{
	for ( int i = 0; i < OverallButtons; i++ ) {
		buttons[i] = nullptr;
	}

	for (int i = 0; i < OverallTextures; i++) {
		textures[i] = nullptr;
	}
}

MainPage::~MainPage()
{
	printf("MAINPAGE DESTRUCT\n");
	renderer = nullptr;

	for (int i = 0; i < OverallButtons; i++) {
		delete buttons[i];
		buttons[i] = NULL;
	}

	for (int i = 0; i < OverallTextures; i++) {
		delete textures[i];
		textures[i] = NULL;
	}

	delete backgroundRenderQuad;
	backgroundRenderQuad = NULL;
	delete background;
	background = NULL;

	sound->free();
}

bool MainPage::init()
{
	if (initialized)
	{
		lastError += "MainPage: already initialized";

		return false;
	}

	textures[BackgroundTexture] = new Texture();
	textures[BackgroundTexture]->loadFromFile(renderer, "./Pics/main_menu.png");

	SDL_Rect* clip = new SDL_Rect{0,0,textures[BackgroundTexture]->getWidth(),textures[BackgroundTexture]->getHeight()};
	background = new Sprite(textures[BackgroundTexture], clip);

	backgroundRenderQuad = new SDL_Rect{ 0, 0, 800, 600 };

	textures[BackgroundAssestTexture] = new Texture();
	textures[BackgroundAssestTexture]->loadFromFile(renderer, "./Pics/main_menu_buttons.png");

	// texture clip coords
	int xButton = 0;
	int yButton = 0;
	int widthButton = 566;
	int heightButton = 84;
	int yHitOffset = 504; // 84 * 6

	// sprite render coords
	int xRenderButton = 257;
	int yRenderButton = 152;
	int widthRenderButton = 283;
	int heightRenderButton = 42;
	int offsetRenderButton = 26;

	sound = MainMenu_Sound::GetInstance();
	sound->init();

	for (int i = 0; i < OverallButtons; i++)
	{
		SDL_Rect* clipOver = new SDL_Rect{ xButton, yButton, widthButton, heightButton };
		Sprite* spriteOver = new Sprite(textures[BackgroundAssestTexture], clipOver);

		SDL_Rect* clipHit = new SDL_Rect{ xButton, yButton + yHitOffset, widthButton, heightButton };
		Sprite* spriteHit = new Sprite(textures[BackgroundAssestTexture], clipHit);

		if (i == ExitButton)
		{
			yRenderButton -= 2;
		}
		SDL_Rect* renderQuad = new SDL_Rect{ xRenderButton, yRenderButton, widthRenderButton, heightRenderButton };

		buttons[i] = new Button(spriteOver, spriteHit, renderQuad, sound->GetOverButtonSound(), sound->GetHitButtonSound());

		yButton += heightButton;
		yRenderButton += heightRenderButton + offsetRenderButton;
	}

	initialized = true;

	return true;
}

bool MainPage::render()
{
	background->render(renderer, backgroundRenderQuad);

	for (int i = 0; i < OverallButtons; i++)
	{
		Button* button = buttons[i];

		if (button->isHit())
		{
			if (!button->isHitRendered())
			{
				button->playHitSound();
			}
			button->renderHit(renderer);

			return true;
		}

		if (button->isOver())
		{
			if (!button->isOverRendered())
			{
				button->playOverSound();
			}
			button->renderOver(renderer);
			
			return true;
		}

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

		int x, y;
		Uint32 mouseState = SDL_GetMouseState(&x, &y);

		if (x > xButton && x < xButton + widthButton)
		{
			for (int i = 0; i < OverallButtons; i++)
			{
				if (y > yButton && y < yButton + heightButton)
				{
					switch (mouseState)
					{
						case 0:
						{
							buttons[i]->over();

							if (buttonStateChanged != i)
							{
								buttons[buttonStateChanged]->resetButtonStates();
								buttonStateChanged = i;
							}

							return;
						}
						case 1:
						{
							buttons[i]->hit();

							if (buttonStateChanged != i)
							{
								buttons[buttonStateChanged]->resetButtonStates();
								buttonStateChanged = i;
							}

							if (i == ExitButton)
							{
								shutdown = true;
							}

							return;
						}
					}
					return;
				}

				yButton += heightButton + offsetButton;
			}
		}
		
		buttons[buttonStateChanged]->resetButtonStates();
	}
}

bool MainPage::isInitialized()
{
	return initialized;
}

bool MainPage::isShutdown()
{
	return shutdown;
}

std::string MainPage::GetLastError()
{
	return lastError;
}
