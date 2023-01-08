#include <SDL.h>
#include <SDL_mixer.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "./src/helpers/StringsHelper.hpp"

#include "./App_Music.hpp"

App_Music* App_Music::_instance = nullptr;

App_Music::App_Music()
    : isInitialized { false }
    , currMusicFile { nullptr }
    , currMusic { nullptr }
{
}

App_Music::~App_Music()
{
    musicPath.clear();

    Mix_FreeMusic(currMusic);
    currMusic = NULL;
}

App_Music* App_Music::GetInstance()
{
    if (_instance == nullptr)
    {
        _instance = new App_Music();
    }

    return _instance;
}

bool App_Music::init()
{
    if (isInitialized)
    {
        lastError += "App_Music init: audio device already initialized";

        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        lastError += StringsHelper::Sprintf(
            "SDL_mixer could not initialize! SDL_mixer Error: %s",
            Mix_GetError()
        );

        return false;
    }

    for (int i = 1; i < 14; i++)
    {
        std::string path = StringsHelper::Sprintf(
            "Music/%d.mp3",
            i
        );

        SDL_RWops* file = SDL_RWFromFile(path.c_str(), "rb");
        if (file == NULL)
        {
            lastError += StringsHelper::Sprintf(
                "Unable to load file %s! SDL Error: %s",
                path.c_str(),
                SDL_GetError()
            );

            return false;
        }

        SDL_FreeRW(file);

        musicPath.push_back(path);
    }

    Mix_HookMusicFinished(playNextMusic);

    itMusicPath = musicPath.begin();

    srand(SDL_GetTicks());
    for (int i = 0; i < rand() % 13; i++)
    {
        itMusicPath++;
    }

    std::string path = *itMusicPath;

    StartMusic();

    isInitialized = true;

    return true;
}

std::string App_Music::GetLastError()
{
    return lastError;
}

void App_Music::NextMusic()
{
    SDL_FreeRW(currMusicFile);
    Mix_FreeMusic(currMusic);

    itMusicPath++;
    if (itMusicPath == musicPath.end())
    {
        itMusicPath = musicPath.begin();
    }
}

void App_Music::StartMusic()
{
    std::string path = *itMusicPath;

    currMusicFile = SDL_RWFromFile(path.c_str(), "rb");
    if (currMusicFile == NULL)
    {
        lastError += StringsHelper::Sprintf(
            "Unable to load file %s! SDL Error: %s",
            path.c_str(),
            SDL_GetError()
        );

        return;
    }

    currMusic = Mix_LoadMUS_RW(currMusicFile,  1);
    if (currMusic == NULL)
    {
        lastError += StringsHelper::Sprintf(
            "Unable to load music %s! SDL_Mixer Error: %s",
            path.c_str(),
            Mix_GetError()
        );

        return;
    }

    if (Mix_PlayingMusic() == 0)
    {
        Mix_PlayMusic(currMusic, 0);
    }
}

void playNextMusic()
{
    printf("music stopped\n");
    App_Music* appMusic = App_Music::GetInstance();

    appMusic->NextMusic();
    appMusic->StartMusic();
}

