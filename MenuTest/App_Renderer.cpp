#include <SDL.h>

#include "./src/helpers/StringsHelper.hpp"

#include "./App_Renderer.hpp"
#include "./App_Window.hpp"

App_Renderer* App_Renderer::_instance = nullptr;

App_Renderer::App_Renderer(App_Window* appWindow)
    : appWindow{ appWindow }
    , renderer{ nullptr }
    , useVSync { false }
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
        lastError += StringsHelper::Sprintf(
            "Renderer could not be created! SDL Error: %s",
            SDL_GetError()
        );

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

bool App_Renderer::ToggleVSync()
{
    useVSync = !useVSync;
    return SDL_RenderSetVSync(renderer, useVSync);
}

bool App_Renderer::IsUsingVSync()
{
    return useVSync;
}


std::string App_Renderer::GetLastError()
{
    return lastError;
}
