#pragma once

#ifndef Game_H
#define Game_H

#include "Logger.hpp"
#include "App_Window.hpp"
#include "App_Renderer.hpp"

class Game
{

protected:

    Game(Logger* alogger);

    ~Game();

    static Game* _instance;

    App_Renderer* appRenderer;
    
    Logger* logger;

public:

    Game(Game& other) = delete;

    void operator=(const Game&) = delete;

    static Game* GetInstance(Logger* alogger);

    bool init();

    bool start();

};

#endif