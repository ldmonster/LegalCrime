#pragma once

#ifndef AppRenderer_H
#define AppRenderer_H

#include <string>
#include <SDL.h>

#include "App_Window.hpp"

class App_Renderer
{

protected:

    App_Renderer(App_Window* appWindow);

    ~App_Renderer();

    static App_Renderer* _instance;

    App_Window* appWindow;

    SDL_Renderer* renderer;

    std::string lastError;

public:

    App_Renderer(App_Renderer& other) = delete;

    void operator=(const App_Renderer&) = delete;

    static App_Renderer* GetInstance(App_Window* appWindow);
    
    bool init();

    SDL_Renderer* GetRenderer();

    std::string GetLastError();

    bool draw();

};

#endif