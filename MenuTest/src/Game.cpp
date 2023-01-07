#include <SDL_mixer.h>

#include "../src/Game.hpp"
#include "../src/Menu.hpp"
#include "../src/logger/Logger.hpp"
#include "../src/helpers/StringsHelper.hpp"

#include "../App_Window.hpp"
#include "../App_Renderer.hpp"
#include "../App_Music.hpp"

Game* Game::_instance = nullptr;

Game::Game(Logger* aLogger)
	: appRenderer { nullptr }
	, logger {aLogger}
	, currSection { MainMenuSection }
	, quit { false }
{
}

Game::~Game()
{
	appRenderer = NULL;
}

Game* Game::GetInstance(Logger* aLogger)
{
	if (_instance == nullptr)
	{
		_instance = new Game(aLogger);
	}

	return _instance;
}

bool Game::init() 
{
	App_Window* appWindow = App_Window::GetInstance(windowWidth, windowHeight, "LOL");
	if ( !appWindow->init() ) 
	{
		logger->LogError( 
			StringsHelper::Sprintf(
				"Game: init window is failed: %s",
				appWindow->GetLastError().c_str()
			)
		);

		return false;
	}

	logger->LogDebug("App_Window init sucessfully");

	if ( !appWindow->SetIconFromFile("Pics/capone.ico") )
	{
		logger->LogError(
			StringsHelper::Sprintf(
				"Game: Window icon is not setted: %s",
				appWindow->GetLastError().c_str()
			)
		);

		return false;
	}

	logger->LogDebug("App_Window icon setted");

	appRenderer = App_Renderer::GetInstance(appWindow);
	if ( !appRenderer->init() )
	{
		logger->LogError(
			StringsHelper::Sprintf(
				"Game: init renderer is failed: %s",
				appWindow->GetLastError().c_str()
			)
		);

		return false;
	}

	logger->LogDebug("App_Renderer init sucessfully");

	App_Music* music = App_Music::GetInstance();
	if (!music->init())
	{
		logger->LogError(
			StringsHelper::Sprintf(
				"Game: init renderer is failed: %s",
				music->GetLastError().c_str()
			)
		);

		return false;
	}

	Mix_Chunk* welcomeChunk = Mix_LoadWAV("Sound/Welcome.wav");
	if (welcomeChunk == NULL)
	{
		logger->LogError(
			StringsHelper::Sprintf(
				"Failed to load beat music! SDL_mixer Error: %s",
				Mix_GetError()
			)
		);

		return false;
	}

	Mix_PlayChannel(1, welcomeChunk, 0);

	logger->LogDebug("Game init sucessfully");

	return true;
}

bool Game::start() 
{
	SDL_Renderer* renderer = appRenderer->GetRenderer();

	logger->LogDebug("Game started");

	GameSection* gameSection = nullptr;

	SDL_Event e;

	while (!quit)
	{
		if (gameSection != nullptr)
		{
			delete gameSection;
			gameSection = nullptr;
		}

		switch (currSection)
		{
			case DefaultSection:
			{
				currSection = MainMenuSection;
				break;
			}
			case MainMenuSection:
			{
				gameSection = new MainPage(renderer);
				break;
			}
			case ShutdownSection:
			{
				return true;
			}
			default:
			{
				currSection = DefaultSection;
				break;
			}
		}

		if (!gameSection->isInitialized())
		{
			if (!gameSection->init())
			{
				logger->LogError(
					StringsHelper::Sprintf(
						"Failed to load game section! Error: %s",
						gameSection->GetLastError()
					)
				);

				return false;
			}
		}

		currSection = eventLoop(gameSection, renderer, &e);
	}

	return true;
}

Uint8 Game::eventLoop(GameSection* gameSection, SDL_Renderer* renderer, SDL_Event* e)
{

	while (!quit)
	{
		// testing destructors
		//delete gameSection;
		//gameSection = nullptr;
		//gameSection = new MainPage(renderer);

		//if (!gameSection->isInitialized())
		//{
		//	if (!gameSection->init())
		//	{
		//		logger->LogError(
		//			StringsHelper::Sprintf(
		//				"Failed to load game section! Error: %s",
		//				gameSection->GetLastError()
		//			)
		//		);

		//		return false;
		//	}
		//}

		while (SDL_PollEvent(e) != 0)
		{
			gameSection->handleEvent(e);

			if (e->type == SDL_QUIT || gameSection->isShutdown())
			{
				quit = true;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		gameSection->render();

		SDL_RenderPresent(renderer);

		SDL_Delay(100);
	}

	SDL_Delay(500);

	return ShutdownSection;
}
