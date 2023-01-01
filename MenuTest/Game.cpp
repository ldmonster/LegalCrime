#include "Game.hpp"
#include "App_Window.hpp"
#include "App_Renderer.hpp"

Game* Game::_instance = nullptr;

Game::Game()
	: appRenderer { nullptr }
{
}

Game::~Game()
{
	appRenderer = nullptr;
}

Game* Game::GetInstance()
{
	if (_instance == nullptr)
	{
		_instance = new Game();
	}

	return _instance;
}


bool Game::init() 
{
	App_Window* appWindow = App_Window::GetInstance(800, 600, "LOL");
	if ( !appWindow->init() ) 
	{
		printf("Game: init window is failed\n");
	}

	if ( !appWindow->SetIconFromFile("Pics/capone.ico") )
	{
		printf("Game: Window icon is not setted\n");
	}

	appRenderer = App_Renderer::GetInstance(appWindow);
	if ( !appRenderer->init() )
	{
		printf("Game: init renderer is failed\n");
	}

	return true;
}

bool Game::start() 
{
	SDL_Renderer* renderer = appRenderer->GetRenderer();

	bool quit = false;

	SDL_Event e;

	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		SDL_RenderPresent(renderer);

		SDL_Delay(100);
	}

	return true;
}