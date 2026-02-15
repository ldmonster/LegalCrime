#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Renderer.hpp"

//------------------------------------------------------------------------------
SDL_Window* renderer::mp_window = nullptr;
SDL_Renderer* renderer::mp_renderer = nullptr;
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
renderer::renderer(size_t aWidth, size_t aHeight, std::string aTitle)
    :m_width{ aWidth }
    , m_height{ aHeight }
    , m_window_title{ aTitle }
{
}
//------------------------------------------------------------------------------
renderer::~renderer()
{
    SDL_DestroyRenderer(mp_renderer);
    SDL_DestroyWindow(mp_window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    m_is_inited = false;
}
//------------------------------------------------------------------------------
bool Renderer::init()
{
    if (mp_window != nullptr)
    {
        m_str_last_error = "Warning: window already created";
        return false;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return m_is_inited;
    }

    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
    {
        m_str_last_error = "Warning: Linear texture filtering not enabled!";
    }

    mp_window = SDL_CreateWindow(m_window_title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        m_width,
        m_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (mp_window == nullptr)
    {
        m_str_last_error = "Window could not be created! SDL_Error:";
        m_str_last_error += SDL_GetError();
        return m_is_inited;
    }

    mp_renderer = SDL_CreateRenderer(mp_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (mp_renderer == nullptr)
    {
        m_str_last_error = "Renderer could not be created! SDL Error:";
        m_str_last_error += SDL_GetError();
        return m_is_inited;
    }

    if (!TTF_WasInit())
    {
        if (TTF_Init() < 0)
        {
            m_str_last_error = "SDL_ttf could not initialize! SDL_ttf Error:";
            m_str_last_error += SDL_GetError();
            return m_is_inited;
        }
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        m_str_last_error = "SDL_image could not initialize! SDL_image Error:";
        m_str_last_error += SDL_GetError();
        return m_is_inited;
    }

    _init_objects();

    m_is_inited = true;
    return m_is_inited;
}

//------------------------------------------------------------------------------
void renderer::_draw()
{
    //Clear screen
    SDL_SetRenderDrawColor(mp_renderer, m_background_r, m_background_g, m_background_b, 0xFF);
    SDL_RenderClear(mp_renderer);

    // draw all objects
    //m_goblin.draw();

    //Update screen
    SDL_RenderPresent(mp_renderer);
}
//------------------------------------------------------------------------------
SDL_Renderer* renderer::sdl_renderer()
{
    return mp_renderer;
}