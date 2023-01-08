#pragma once

#ifndef Game_H
#define Game_H

#include "../src/logger/Logger.hpp"
#include "../App_Window.hpp"
#include "../App_Renderer.hpp"

class GameSection
{

protected:

public:

    virtual ~GameSection() {};

    virtual bool init() = 0;
    virtual bool render() = 0;
    virtual void handleEvent(SDL_Event* e) = 0;

    virtual bool isInitialized() = 0;
    virtual bool isShutdown() = 0;

    virtual std::string GetLastError() = 0;

};

class Game
{

protected:

    enum GameSections {
        DefaultSection,
        OpeningSection,
        MainMenuSection,
        PlayingGameSection,
        ShutdownSection
    };

    Game(Logger* alogger);

    ~Game();

    static Game* _instance;

    Uint16 windowWidth = 800;
    Uint16 windowHeight = 600;

    bool useVSync;

    App_Renderer* appRenderer;
    
    Logger* logger;

    Uint8 currSection;

    bool quit;

public:

    Game(Game& other) = delete;

    void operator=(const Game&) = delete;

    static Game* GetInstance(Logger* alogger);

    bool init();

    bool start();

    Uint8 eventLoop(GameSection* gameSection, SDL_Renderer* renderer, SDL_Event* e);

};

#endif