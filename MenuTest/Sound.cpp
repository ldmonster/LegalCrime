#include <SDL_mixer.h>

#include "./src/helpers/StringsHelper.hpp"

#include "./Sound.hpp"

Sound::Sound()
{
}

MainMenu_Sound* MainMenu_Sound::_instance = nullptr;

MainMenu_Sound::MainMenu_Sound()
    : isInitialized{ false }
    , overButtonSound{ nullptr }
    , hitButtonSound { nullptr }
{
}

MainMenu_Sound::~MainMenu_Sound()
{
    printf("MAINPAGE SOUND DESTRUCT\n");

    Mix_FreeChunk(overButtonSound);
    Mix_FreeChunk(hitButtonSound);
    overButtonSound = NULL;
    hitButtonSound = NULL;

    free();
}

MainMenu_Sound* MainMenu_Sound::GetInstance()
{
    if (_instance == nullptr)
    {
        _instance = new MainMenu_Sound();
    }

    return _instance;
}

bool MainMenu_Sound::init()
{
    if (isInitialized)
    {
        return false;
    }

    overButtonSound = Mix_LoadWAV("Sound/OverButton.wav");
    if (overButtonSound == NULL)
    {
        lastError += StringsHelper::Sprintf(
            "Failed to load over button sound effect! SDL_mixer Error: %s",
            Mix_GetError()
        );

        return false;
    }

    hitButtonSound = Mix_LoadWAV("Sound/ButtonClick.wav");
    if (hitButtonSound == NULL)
    {
        lastError += StringsHelper::Sprintf(
            "Failed to load hit button sound effect! SDL_mixer Error: %s",
            Mix_GetError()
        );

        return false;
    }

    isInitialized = true;

    return true;
}

void MainMenu_Sound::free()
{
    Mix_FreeChunk(overButtonSound);
    Mix_FreeChunk(hitButtonSound);
    overButtonSound = nullptr;
    hitButtonSound = nullptr;

    isInitialized = false;
}

Mix_Chunk* MainMenu_Sound::GetOverButtonSound()
{
    return overButtonSound;
}

Mix_Chunk* MainMenu_Sound::GetHitButtonSound()
{
    return hitButtonSound;
}

std::string MainMenu_Sound::GetLastError()
{
    return lastError;
}

