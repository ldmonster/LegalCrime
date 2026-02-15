#pragma once

#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <mutex>
#include <sstream>
#include <iomanip>

#if __cplusplus >= 202002L
#include <source_location>
#endif

enum class LogPriority {
    TraceP, DebugP, InfoP, WarnP, ErrorP, FatalP
};

class Logger
{
protected:
    LogPriority verbosity;
    std::mutex log_mutex;

public:
    virtual ~Logger() = default;

    virtual void SetVerbosity(LogPriority new_priority) = 0;

    virtual const char* GetVerbocityStr(LogPriority priority) noexcept = 0;

    virtual void Log(LogPriority priority, const std::string& message) = 0;

#if __cplusplus >= 202002L
    virtual void Log(LogPriority priority, const std::string& message, 
                    std::source_location location = std::source_location::current()) = 0;
#endif

    virtual void LogDebug(const std::string& message) = 0;
    virtual void LogError(const std::string& message) = 0;
};

class GameLogger : public Logger {
public:
    GameLogger();
    ~GameLogger() override;

    void SetVerbosity(LogPriority new_priority) override;

    const char* GetVerbocityStr(LogPriority priority) noexcept override;

    void Log(LogPriority priority, const std::string& message) override;

#if __cplusplus >= 202002L
    void Log(LogPriority priority, const std::string& message, std::source_location location) override;
#endif

    void LogDebug(const std::string& message) override;
    void LogError(const std::string& message) override;

private:
    std::string getCurrentTime();

    void formatAndOutput(LogPriority priority, const std::string& message
#if __cplusplus >= 202002L
                        , std::source_location location = std::source_location::current()
#endif
                        );
};


