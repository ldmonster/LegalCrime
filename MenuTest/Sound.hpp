#pragma once

#ifndef Sound_H
#define Sound_H

#include <string>

#include <SDL_mixer.h>

class Sound
{

protected:

    Sound();

public:

    virtual ~Sound() {};

    Sound(Sound& other) = delete;

    void operator=(const Sound&) = delete;

    static Sound* GetInstance();

    virtual bool init() = 0;
    virtual void free() = 0;

    virtual std::string GetLastError() = 0;

};

class MainMenu_Sound: public Sound
{

protected:

    MainMenu_Sound();

    static MainMenu_Sound* _instance;

    bool isInitialized;

    Mix_Chunk* overButtonSound;
    Mix_Chunk* hitButtonSound;

    std::string lastError;

public:

    ~MainMenu_Sound();

    MainMenu_Sound(MainMenu_Sound& other) = delete;

    void operator=(const MainMenu_Sound&) = delete;

    static MainMenu_Sound* GetInstance();

    bool init();
    void free();

    Mix_Chunk* GetOverButtonSound();
    Mix_Chunk* GetHitButtonSound();

    std::string GetLastError();

};

#endif
