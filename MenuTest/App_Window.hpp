#pragma once

#ifndef AppWindow_H
#define AppWindow_H

#include <string>
#include <SDL.h>

class App_Window
{

protected:

    App_Window(Uint32 aWidth, Uint32 aHeight, std::string aTitle);

    ~App_Window();

    static App_Window* _instance;

    SDL_Window* window;

    Uint32 width{ 0 };
    Uint32 height{ 0 };

    std::string title{""};

    std::string lastError;

    SDL_Surface* icon = NULL;

public:

    App_Window(App_Window& other) = delete;

    void operator=(const App_Window&) = delete;

    static App_Window* GetInstance(Uint32 aWidth, Uint32 aHeight, std::string aTitle);

    bool init();

    SDL_Window* GetWindow();

    bool SetIconFromFile(std::string aPath);

    std::string GetLastError();

};

#endif