#ifndef Game_H
#define Game_H

#include <string>
#include <SDL.h>

#include "App_Window.hpp"
#include "App_Renderer.hpp"

class Game
{

protected:

    Game();

    ~Game();

    static Game* _instance;

    App_Renderer* appRenderer;

public:

    Game(Game& other) = delete;

    void operator=(const Game&) = delete;

    static Game* GetInstance();

    bool init();

    bool start();

};

#endif