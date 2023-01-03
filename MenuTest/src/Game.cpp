#include "../src/Game.hpp"
#include "../src/Menu.hpp"
#include "../src/logger/Logger.hpp"
#include "../src/helpers/StringsHelper.hpp"

#include "../App_Window.hpp"
#include "../App_Renderer.hpp"

Game* Game::_instance = nullptr;

Game::Game(Logger* aLogger)
	: appRenderer { nullptr }
	, logger {aLogger}
{
}

Game::~Game()
{
	appRenderer = nullptr;
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

	logger->LogDebug("Game init sucessfully");

	return true;
}

bool Game::start() 
{
	SDL_Renderer* renderer = appRenderer->GetRenderer();

	logger->LogDebug("Game started");

	bool quit = false;

	SDL_Event e;

	MenuPage* mainMenu = new MainPage(renderer);
	mainMenu->init();

	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			mainMenu->handleEvent(&e);
		}
		
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		mainMenu->render();

		SDL_RenderPresent(renderer);
		SDL_Delay(100);
	}

	return true;
}