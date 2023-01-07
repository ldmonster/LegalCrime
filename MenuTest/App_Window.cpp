#include <SDL_image.h>

#include "./src/helpers/StringsHelper.hpp"

#include "./App_Window.hpp"

App_Window* App_Window::_instance = nullptr;

App_Window::App_Window(Uint32 aWidth, Uint32 aHeight, std::string aTitle)
    : width{ aWidth }
    , height{ aHeight }
    , title{ aTitle }
    , window{ nullptr }
{
}

App_Window::~App_Window() 
{
    SDL_FreeSurface(icon);
    icon = NULL;

    SDL_DestroyWindow(window);
    window = NULL;
}

bool App_Window::init() 
{
    if (window != nullptr)
    {
        lastError += "App_Window init: window already initialized";

        return false;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        lastError += StringsHelper::Sprintf(
            "App_Window init: SDL could not initialize! SDL_Error: %s",
            SDL_GetError()
        );

        return false;
    }

    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
    {
        lastError += StringsHelper::Sprintf(
            "App_Window init: Linear texture filtering not enabled!"
        );
    }

    window = SDL_CreateWindow(title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (window == nullptr)
    {
        lastError += StringsHelper::Sprintf(
            "App_Window init: Window could not be created! SDL_Error: %s",
            SDL_GetError()
        );

        return false;
    }

	return true;
}

App_Window* App_Window::GetInstance(Uint32 aWidth, Uint32 aHeight, std::string aTitle) 
{
    if (_instance == nullptr) 
    {
        _instance = new App_Window(aWidth,aHeight,aTitle);
    }

    return _instance;
}

SDL_Window* App_Window::GetWindow() 
{
    return window;
}

bool App_Window::SetIconFromFile(std::string aPath)
{
    SDL_RWops* file = SDL_RWFromFile(aPath.c_str(), "rb");
    if (file == NULL)
    {
        lastError += StringsHelper::Sprintf(
            "Unable to load file %s! SDL_image Error: %s",
            aPath.c_str(),
            SDL_GetError() 
        );

        return false;
    }

    SDL_Surface* loadedSurface = IMG_LoadICO_RW(file);
    if (loadedSurface == NULL)
    {
        lastError += StringsHelper::Sprintf(
            "Unable to load image %s! SDL_image Error: %s",
            aPath.c_str(),
            IMG_GetError()
        );

        return false;
    }

    SDL_SetWindowIcon(window, loadedSurface);

    SDL_FreeRW(file);

    return true;
}

std::string App_Window::GetLastError()
{
    return lastError;
}
