#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

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

    MIX_DestroyAudio(overButtonSound);
    MIX_DestroyAudio(hitButtonSound);
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

    SDL_IOStream* overRw = SDL_IOFromFile("Sound/OverButton.wav", "rb");
    if (overRw == NULL)
    {
        lastError += StringsHelper::Sprintf(
            "Failed to open over button sound file! SDL Error: %s",
            SDL_GetError()
        );

        return false;
    }

    overButtonSound = MIX_LoadAudio_IO(nullptr, overRw, true, NULL);
    if (overButtonSound == NULL)
    {
        lastError += StringsHelper::Sprintf(
            "Failed to load over button sound effect! SDL Error: %s",
            SDL_GetError()
        );

        return false;
    }

    SDL_IOStream* hitRw = SDL_IOFromFile("Sound/ButtonClick.wav", "rb");
    if (hitRw == NULL)
    {
        lastError += StringsHelper::Sprintf(
            "Failed to open hit button sound file! SDL Error: %s",
            SDL_GetError()
        );

        return false;
    }

    hitButtonSound = MIX_LoadAudio_IO(nullptr, hitRw, true, NULL);
    if (hitButtonSound == NULL)
    {
        lastError += StringsHelper::Sprintf(
            "Failed to load hit button sound effect! SDL Error: %s",
            SDL_GetError()
        );

        return false;
    }

    isInitialized = true;

    return true;
}

void MainMenu_Sound::free()
{
    MIX_DestroyAudio(overButtonSound);
    MIX_DestroyAudio(hitButtonSound);
    overButtonSound = nullptr;
    hitButtonSound = nullptr;

    isInitialized = false;
}

MIX_Audio* MainMenu_Sound::GetOverButtonSound()
{
    return overButtonSound;
}

MIX_Audio* MainMenu_Sound::GetHitButtonSound()
{
    return hitButtonSound;
}

std::string MainMenu_Sound::GetLastError()
{
    return lastError;
}

