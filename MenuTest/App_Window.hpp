#pragma once

#ifndef AppWindow_H
#define AppWindow_H

#include <string>
#include <SDL.h>

enum ScreenResolutions {
    SR_800x600,
    SR_1024x768,
    SR_1280x720,
    SR_1360x768,
    SR_1920x1080,
};

class App_Window
{

protected:

    App_Window();

    ~App_Window();

    static App_Window* _instance;

    SDL_Window* m_window;

    Uint16 m_width{ 0 };
    Uint16 m_height{ 0 };

    std::string m_title{""};

    std::string m_lastError;

    SDL_Surface* m_icon = NULL;

    void SetResolution(ScreenResolutions sr);

public:

    App_Window(App_Window& other) = delete;

    void operator=(const App_Window&) = delete;

    static App_Window* GetInstance();

    bool init(ScreenResolutions sr, std::string aTitle);

    SDL_Window* GetWindow();

    bool SetIconFromFile(std::string aPath);

    Uint32 GetWidth();
    Uint32 GetHeight();

    std::string GetLastError();

};

#endif