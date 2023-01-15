#include <SDL.h>
#include <SDL_mixer.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "./src/helpers/StringsHelper.hpp"
#include "./src/helpers/OsHelper.hpp"

#include "./App_Music.hpp"

App_Music* App_Music::_instance = nullptr;

App_Music::App_Music()
    : isInitialized { false }
    , m_currMusicFile { nullptr }
    , m_currMusic{ nullptr }
{
}

App_Music::~App_Music()
{
    musicPath.clear();

    Mix_FreeMusic(m_currMusic);
    m_currMusic = NULL;
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
        m_lastError += "App_Music init: audio device already initialized";

        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        m_lastError += StringsHelper::Sprintf(
            "SDL_mixer could not initialize! SDL_mixer Error: %s",
            Mix_GetError()
        );

        return false;
    }

    musicPath = directory_files("./Music/", true);
    
    Mix_HookMusicFinished(playNextMusic);

    itMusicPath = musicPath.begin();

    StartMusic();

    printf("last error: %s\n", m_lastError.c_str());

    isInitialized = true;

    return true;
}

std::string App_Music::GetLastError()
{
    return m_lastError;
}


void App_Music::RandMusic()
{
    SDL_FreeRW(m_currMusicFile);
    Mix_FreeMusic(m_currMusic);

    itMusicPath = musicPath.begin();

    srand(SDL_GetTicks());

    itMusicPath+= (rand() % (musicPath.size() - 1)) + 1;

    StartMusic();
}

void App_Music::NextMusic()
{
    SDL_FreeRW(m_currMusicFile);
    Mix_FreeMusic(m_currMusic);

    itMusicPath++;
    if (itMusicPath == musicPath.end())
    {
        itMusicPath = musicPath.begin();
        return;
    }

    StartMusic();
}

void App_Music::StartMusic()
{
    SDL_RWops* rw = SDL_RWFromFile(itMusicPath->c_str(), "rb");
    if (rw == NULL)
    {
        m_lastError += StringsHelper::Sprintf(
            "Unable to load file %s! SDL Error: %s",
            itMusicPath->c_str(),
            SDL_GetError()
        );

        return;
    }

    Mix_Music* nextMusic = Mix_LoadMUS_RW(rw,  1);
    if (nextMusic == NULL)
    {
        m_lastError += StringsHelper::Sprintf(
            "Unable to load music %s! SDL_Mixer Error: %s",
            itMusicPath->c_str(),
            Mix_GetError()
        );

        return;
    }

    m_currMusicFile = rw;
    m_currMusic = nextMusic;

    if (Mix_PlayingMusic() == 0)
    {
        Mix_PlayMusic(m_currMusic, 0);
    }
}

void playNextMusic()
{
    printf("music stopped\n");
    App_Music* appMusic = App_Music::GetInstance();

    appMusic->NextMusic();
}

