#include <SDL.h>

#include "App_Renderer.hpp"
#include "App_Window.hpp"

App_Renderer* App_Renderer::_instance = nullptr;

App_Renderer::App_Renderer(App_Window* appWindow)
    : appWindow{ appWindow }
    , renderer{ nullptr }
{
}

App_Renderer::~App_Renderer()
{
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    appWindow = nullptr;
}

bool App_Renderer::init()
{
	renderer = SDL_CreateRenderer(appWindow->GetWindow(), -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

    return true;
}


App_Renderer* App_Renderer::GetInstance(App_Window* appWindow) {
    if (_instance == nullptr) {
        _instance = new App_Renderer(appWindow);
    }

    return _instance;
}

SDL_Renderer* App_Renderer::GetRenderer()
{
    return renderer;
}