#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "./src/helpers/StringsHelper.hpp"
#include "./src/helpers/OsHelper.hpp"

#include "./App_Music.hpp"

void playNextMusicCallback(void* userdata, MIX_Track* track);

App_Music* App_Music::_instance = nullptr;

App_Music::App_Music()
    : isInitialized { false }
    , m_currMusicFile { nullptr }
    , m_currMusic{ nullptr }
    , m_mixer{ nullptr }
    , m_track{ nullptr }
{
}

App_Music::~App_Music()
{
    musicPath.clear();

    if (m_track) {
        MIX_StopTrack(m_track,0);
        MIX_DestroyTrack(m_track);
    }
    if (m_mixer) {
        MIX_DestroyMixer(m_mixer);
    }
    if (m_currMusic) {
        MIX_DestroyAudio(m_currMusic);
    }
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

    if (!MIX_Init())
    {
        m_lastError += StringsHelper::Sprintf(
            "SDL_mixer could not initialize! SDL_mixer Error: %s",
            SDL_GetError()
        );

        return false;
    }

    // In SDL3_mixer, use SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK for default audio device
    m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!m_mixer)
    {
        m_lastError += StringsHelper::Sprintf(
            "SDL_mixer could not create mixer! SDL_mixer Error: %s",
            SDL_GetError()
        );

        return false;
    }

    m_track = MIX_CreateTrack(m_mixer);
    if (!m_track)
    {
        m_lastError += StringsHelper::Sprintf(
            "SDL_mixer could not create track! SDL_mixer Error: %s",
            SDL_GetError()
        );

        return false;
    }

    musicPath = directory_files("./Music/", true);

    MIX_SetTrackStoppedCallback(m_track, nullptr, playNextMusicCallback);

    itMusicPath = musicPath.begin();

    StartMusic();

    isInitialized = true;

    return true;
}

std::string App_Music::GetLastError()
{
    return m_lastError;
}


void App_Music::RandMusic()
{
    MIX_StopTrack(m_track, 0);
    MIX_DestroyAudio(m_currMusic);

    itMusicPath = musicPath.begin();

    srand(SDL_GetTicks());

    itMusicPath+= (rand() % (musicPath.size() - 1)) + 1;

    StartMusic();
}

void App_Music::NextMusic()
{
    MIX_StopTrack(m_track, 0);
    MIX_DestroyAudio(m_currMusic);

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
    SDL_IOStream* rw = SDL_IOFromFile(itMusicPath->c_str(), "rb");
    if (rw == NULL)
    {
        m_lastError += StringsHelper::Sprintf(
            "Unable to load file %s! SDL Error: %s",
            itMusicPath->c_str(),
            SDL_GetError()
        );

        return;
    }

    MIX_Audio* nextAudio = MIX_LoadAudio_IO(m_mixer, rw, true, NULL);
    if (nextAudio == NULL)
    {
        m_lastError += StringsHelper::Sprintf(
            "Unable to load music %s! SDL_Mixer Error: %s",
            itMusicPath->c_str(),
            SDL_GetError()
        );

        return;
    }

    m_currMusicFile = rw;
    m_currMusic = nextAudio;

    MIX_SetTrackAudio(m_track, m_currMusic);

    if (MIX_TrackPlaying(m_track) == 0)
    {
        MIX_PlayTrack(m_track, 0);
    }
}

void playNextMusicCallback(void* userdata, MIX_Track* track)
{
    printf("music stopped\n");
    App_Music* appMusic = App_Music::GetInstance();

    appMusic->NextMusic();
}

