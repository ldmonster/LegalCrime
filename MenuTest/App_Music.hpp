#pragma once

#ifndef AppMusic_H
#define AppMusic_H

#include <string>
#include <vector>

#include <SDL_mixer.h>

class App_Music
{

protected:

    App_Music();

    ~App_Music();

    static App_Music* _instance;

    bool isInitialized;

    std::vector <std::string> musicPath;
    std::vector<std::string>::iterator itMusicPath;

    SDL_RWops* currMusicFile;
    Mix_Music* currMusic;

    std::string lastError;

public:

    App_Music(App_Music& other) = delete;

    void operator=(const App_Music&) = delete;

    static App_Music* GetInstance();

    bool init();

    std::string GetLastError();

    void StartMusic();
    void NextMusic();
    void RandMusic();

};

void playNextMusic();

#endif

