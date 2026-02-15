#include <SDL3_image/SDL_image.h>

#include "./src/helpers/StringsHelper.hpp"

#include "./App_Window.hpp"

App_Window* App_Window::_instance = nullptr;

App_Window::App_Window()
{
    m_width = 0;
    m_height = 0;
    m_title = "";
    m_window = nullptr;

}

App_Window::~App_Window() 
{
    SDL_DestroySurface(m_icon);
    m_icon = NULL;

    SDL_DestroyWindow(m_window);
    m_window = NULL;
}

void App_Window::SetResolution(ScreenResolutions sr)
{
    switch (sr)
    {
    case SR_800x600:
        m_width = 800;
        m_height = 600;
        break;
    case SR_1024x768:
        m_width = 1024;
        m_height = 768;
        break;
    case SR_1280x720:
        m_width = 1280;
        m_height = 720;
        break;
    case SR_1360x768:
        m_width = 1360;
        m_height = 768;
        break;
    case SR_1920x1080:
        m_width = 1920;
        m_height = 1080;
        break;
    default:
        break;
    }
}


bool App_Window::init(ScreenResolutions sr, std::string aTitle)
{
    SetResolution(sr);
    m_title = aTitle;

    if (m_window != nullptr)
    {
        m_lastError += "App_Window init: window already initialized";

        return false;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        m_lastError += StringsHelper::Sprintf(
            "App_Window init: SDL could not initialize! SDL_Error: %s",
            SDL_GetError()
        );

        return false;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, m_title.c_str());
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_UNDEFINED);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_UNDEFINED);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, m_width);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, m_height);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, SDL_WINDOW_RESIZABLE);
    m_window = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);

    if (m_window == nullptr)
    {
        m_lastError += StringsHelper::Sprintf(
            "App_Window init: Window could not be created! SDL_Error: %s",
            SDL_GetError()
        );

        return false;
    }

	return true;
}

App_Window* App_Window::GetInstance() 
{
    if (_instance == nullptr) 
    {
        _instance = new App_Window();
    }

    return _instance;
}

SDL_Window* App_Window::GetWindow() 
{
    return m_window;
}

bool App_Window::SetIconFromFile(std::string aPath)
{
    SDL_IOStream* file = SDL_IOFromFile(aPath.c_str(), "rb");
    if (file == NULL)
    {
        m_lastError += StringsHelper::Sprintf(
            "Unable to load file %s! SDL_image Error: %s",
            aPath.c_str(),
            SDL_GetError() 
        );

        return false;
    }

    SDL_Surface* loadedSurface = IMG_LoadICO_IO(file);
    if (loadedSurface == NULL)
    {
        m_lastError += StringsHelper::Sprintf(
            "Unable to load image %s! SDL_image Error: %s",
            aPath.c_str(),
            SDL_GetError()
        );

        return false;
    }

    SDL_SetWindowIcon(m_window, loadedSurface);

    SDL_CloseIO(file);

    return true;
}

Uint32 App_Window::GetWidth() 
{
    return m_width;
}

Uint32 App_Window::GetHeight() 
{
    return m_height;
}

std::string App_Window::GetLastError()
{
    return m_lastError;
}
