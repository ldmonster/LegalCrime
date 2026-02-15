#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "../src/Game.hpp"
#include "../src/Gameplay.hpp"
#include "../src/Menu.hpp"
#include "../src/logger/Logger.hpp"
#include "../src/helpers/StringsHelper.hpp"

#include "../App_Window.hpp"
#include "../App_Renderer.hpp"
#include "../App_Music.hpp"
#include "../App_FpsController.hpp"

Game* Game::_instance = nullptr;

Game::Game(Logger* aLogger)
	: appRenderer { nullptr }
	, logger {aLogger}
	, useVSync { false }
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
	App_Window* appWindow = App_Window::GetInstance();
	if ( !appWindow->init(SR_800x600, "Legal Crime"))
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
	//appRenderer->ToggleVSync();

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

	// TODO: SDL3_mixer uses MIX_Audio and tracks instead of Mix_Chunk and channels
	// Mix_Chunk* welcomeChunk = Mix_LoadWAV("Sound/Welcome.wav");
	// if (welcomeChunk == NULL)
	// {
	// 	logger->LogError(
	// 		StringsHelper::Sprintf(
	// 			"Failed to load beat music! SDL_mixer Error: %s",
	// 			Mix_GetError()
	// 		)
	// 	);
	//
	// 	return false;
	// }
	//
	// Mix_PlayChannel(1, welcomeChunk, 0);

	if (TTF_Init() == -1)
	{
		logger->LogError(
			StringsHelper::Sprintf(
				"SDL_ttf could not initialize! SDL_ttf Error: %s",
				SDL_GetError()
			)
		);

		return false;
	}

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
			case PlayingGameSection:
			{
				App_Music* music = App_Music::GetInstance();
				music->RandMusic();

				gameSection = new Gameplay(renderer);
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

		if (gameSection == nullptr)
		{
			continue;
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

GameSections Game::eventLoop(GameSection* gameSection, SDL_Renderer* renderer, SDL_Event* e)
{
	GameSections newGameSection;

	printf("is using vsync %d\n", appRenderer->IsUsingVSync());

	FpsController* fpsController;
	fpsController = App_FpsController::GetInstance(appRenderer->IsUsingVSync());
	if (!fpsController->init(renderer))
	{
		printf("Can not initialize Fps Controller");
	}

	fpsController->FpsCounterStart();

	while (!quit)
	{
		fpsController->CapTicksCounterStart();

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

			if (e->type == SDL_EVENT_QUIT)
			{
				quit = true;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		gameSection->render();
		fpsController->render(renderer);

		SDL_RenderPresent(renderer);

		fpsController->FpsCountAdd();

		fpsController->CappingFrame();

		newGameSection = gameSection->GetGameSection();
		if (newGameSection != currSection)
		{
			break;
		}
	}

	SDL_Delay(500);

	return newGameSection;
}
