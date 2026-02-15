#pragma once

#ifndef AppFpsController_H
#define AppFpsController_H

#include <string>
#include <SDL3/SDL.h>

#include "src/Texture.hpp"
#include "src/helpers/StringsHelper.hpp"

class SimpleTimer
{
public:
	SimpleTimer();

	void start();
	void stop();
	void pause();
	void unpause();

	Uint64 getTicks();

	bool isStarted();
	bool isPaused();

private:
	Uint64 m_startTicks;

	Uint64 m_pausedTicks;

	bool m_paused;
	bool m_started;
};

class FpsController
{

protected:

    FpsController() {};
    virtual ~FpsController() {};

public:

    FpsController(FpsController& other) = delete;

    void operator=(const FpsController&) = delete;

    static FpsController* GetInstance();

    virtual bool init(SDL_Renderer* renderer) = 0;

    virtual void FpsCounterStart() = 0;
    virtual void FpsCountAdd() = 0;

    virtual void CapTicksCounterStart() = 0;
    virtual void CappingFrame() = 0;

    virtual  void SetFpsCap(Uint8 fpsCap) = 0;
    virtual Uint16 GetAverageFps() = 0;

    virtual void render(SDL_Renderer* renderer) = 0;

    virtual std::string GetLastError() = 0;

};

FpsController* FpsController::GetInstance()
{
    return NULL;
}

class App_FpsController: public FpsController
{

protected:

    App_FpsController();

    ~App_FpsController();

    static App_FpsController* _instance;

    Uint8 m_screenFpsCap;
    Uint32 m_screenTickPerFrame;

    SimpleTimer* m_fpsTimer;
    SimpleTimer* m_capTimer;

    Uint16 m_averageFps;

    Uint32 m_countedFrames;

    Texture* m_fpsTextTexture;
    Texture* m_fpsTextCounterTexture;
    SDL_Color m_fpsTextColor;
    SDL_Rect* m_fpsTextRenderQuad;
    SDL_Rect* m_fpsTextCounterRenderQuad;

    std::string m_lastError;

public:

    App_FpsController(App_FpsController& other) = delete;

    void operator=(const App_FpsController&) = delete;

    static App_FpsController* GetInstance(bool useVSync);

    bool init(SDL_Renderer* renderer);

    void FpsCounterStart();
    void FpsCountAdd();

    void CapTicksCounterStart();
    void CappingFrame();

    void SetFpsCap(Uint8 fpsCap);
    Uint16 GetAverageFps();

    void render(SDL_Renderer* renderer);

    std::string GetLastError();

};


class App_FpsControllerVSync : public App_FpsController
{

protected:

    App_FpsControllerVSync(){};

    static App_FpsControllerVSync* _instance;

    std::string m_lastError;

public:

    App_FpsControllerVSync(App_FpsControllerVSync& other) = delete;

    void operator=(const App_FpsControllerVSync&) = delete;

    static App_FpsControllerVSync* GetInstance();

    void CapTicksCounterStart();
    void CappingFrame();

    void SetFpsCap(Uint8 fpsCap);
    
};

#endif

/////////////////////////////////////////////////////////////
///         SimpleTimer
/////////////////////////////////////////////////////////////

SimpleTimer::SimpleTimer()
{
    m_startTicks = 0;
    m_pausedTicks = 0;

    m_paused = false;
    m_started = false;
}

void SimpleTimer::start()
{
    m_started = true;
    m_paused = false;

    m_startTicks = SDL_GetTicks();
    m_pausedTicks = 0;
}

void SimpleTimer::stop()
{
    m_started = false;
    m_paused = false;

    m_startTicks = 0;
    m_pausedTicks = 0;
}

void SimpleTimer::pause()
{
    if (m_started && !m_paused)
    {
        m_paused = true;

        m_pausedTicks = SDL_GetTicks() - m_startTicks;
        m_startTicks = 0;
    }
}

void SimpleTimer::unpause()
{
    if (m_started && m_paused)
    {
        m_paused = false;

        m_startTicks = SDL_GetTicks() - m_pausedTicks;

        m_pausedTicks = 0;
    }
}

Uint64 SimpleTimer::getTicks()
{
    Uint64 time = 0;

    if (m_started)
    {
        if (m_paused)
        {
            time = m_pausedTicks;
        }
        else
        {
            time = SDL_GetTicks() - m_startTicks;
        }
    }

    return time;
}

bool SimpleTimer::isStarted()
{
    return m_started;
}

bool SimpleTimer::isPaused()
{
    return m_paused && m_started;
}

/////////////////////////////////////////////////////////////
///         App_FpsController
/////////////////////////////////////////////////////////////

App_FpsController* App_FpsController::_instance = nullptr;

App_FpsController::App_FpsController()
{
    m_fpsTimer = nullptr;
    m_capTimer = nullptr;

    m_averageFps = 0;
    m_countedFrames = 0;

    m_fpsTextTexture = nullptr;
    m_fpsTextCounterTexture = nullptr;
    m_fpsTextRenderQuad = nullptr;
    m_fpsTextCounterRenderQuad = nullptr;
    m_fpsTextColor = {};
}

App_FpsController::~App_FpsController()
{
    delete m_fpsTimer;
    delete m_capTimer;

    delete m_fpsTextTexture;
    delete m_fpsTextCounterTexture;
    delete m_fpsTextRenderQuad;
    delete m_fpsTextCounterRenderQuad;

    m_fpsTimer = NULL;
    m_capTimer = NULL;

    m_averageFps = 0;
    m_countedFrames = 0;
    m_screenFpsCap = 0;
    m_screenTickPerFrame = 0;
}

App_FpsController* App_FpsController::GetInstance(bool useVSync)
{
    if (useVSync)
    {
        return App_FpsControllerVSync::GetInstance();
    }

    if (_instance == nullptr)
    {
        _instance = new App_FpsController();
    }

    return _instance;
}

bool App_FpsController::init(SDL_Renderer* renderer)
{
    m_fpsTimer = new SimpleTimer();
    m_capTimer = new SimpleTimer();

    SetFpsCap(60);

    m_fpsTextTexture = new Texture();
    m_fpsTextTexture->SetFont("Fonts/OpenSans-ExtraBold.ttf", 28);
    m_fpsTextRenderQuad = new SDL_Rect{ 710, 0, 40, 35 };

    m_fpsTextCounterTexture = new Texture();
    m_fpsTextCounterTexture->SetFont("Fonts/OpenSans-ExtraBold.ttf", 28);
    m_fpsTextCounterRenderQuad = new SDL_Rect{ 755, 0, 40, 35 };

    m_fpsTextColor = { 255, 255, 0, 255 };

    if (!m_fpsTextTexture->loadFromRenderedText(renderer, "fps", m_fpsTextColor))
    {
        printf("Unable to render FPS text texture!\n");

        return false;
    }

    return true;
}


void App_FpsController::SetFpsCap(Uint8 fpsCap)
{
    m_screenFpsCap = fpsCap;
    m_screenTickPerFrame = 1000000000LL / m_screenFpsCap;
}

void App_FpsController::FpsCounterStart()
{
    m_countedFrames = 0;
    m_fpsTimer->start();
}

void App_FpsController::FpsCountAdd()
{
    ++m_countedFrames;
}

Uint16 App_FpsController::GetAverageFps()
{
    m_averageFps = m_countedFrames / (m_fpsTimer->getTicks() / 1000000000.0);
    if (m_averageFps > 2000000)
    {
        m_averageFps = 0;
    }

    return m_averageFps;
}

void App_FpsController::CapTicksCounterStart()
{
    m_capTimer->start();
}

void App_FpsController::CappingFrame()
{
    Uint64 frameTicks = m_capTimer->getTicks();
    if (frameTicks < m_screenTickPerFrame)
    {
        SDL_DelayNS(m_screenTickPerFrame - frameTicks);
    }
}

void App_FpsController::render(SDL_Renderer* renderer)
{
    std::string currFps = StringsHelper::Sprintf(
        "%d",
        GetAverageFps()
    );

    if (!m_fpsTextCounterTexture->loadFromRenderedText(renderer, currFps, m_fpsTextColor))
    {
        printf("Unable to render FPS texture!\n");
    }

    m_fpsTextTexture->render(renderer, m_fpsTextRenderQuad);
    m_fpsTextCounterTexture->render(renderer, m_fpsTextCounterRenderQuad);
}

std::string App_FpsController::GetLastError()
{
    return m_lastError;
}

/////////////////////////////////////////////////////////////
///         App_FpsControllerVSync
/////////////////////////////////////////////////////////////

App_FpsControllerVSync* App_FpsControllerVSync::_instance = nullptr;

App_FpsControllerVSync* App_FpsControllerVSync::GetInstance()
{
    if (_instance == nullptr)
    {
        _instance = new App_FpsControllerVSync();
    }

    return _instance;
}

void App_FpsControllerVSync::CapTicksCounterStart(){}

void App_FpsControllerVSync::CappingFrame() {
}

void App_FpsControllerVSync::SetFpsCap(Uint8 fpsCap) {}


