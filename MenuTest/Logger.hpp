#pragma once

#ifndef Logger_H
#define Logger_H

#include <string>

enum LogPriority {
	TraceP, DebugP, InfoP, WarnP, ErrorP, FatalP
};

class Logger
{

protected:

    LogPriority verbosity;

public:

    virtual void SetVerbosity(LogPriority new_priority) = 0;

    virtual const char* GetVerbocityStr(LogPriority priority) = 0;

    virtual void Log(LogPriority priority, std::string message) = 0;

    virtual void LogDebug(std::string message) = 0;
    virtual void LogError(std::string message) = 0;

};

class GameLogger : public Logger {

protected:

    LogPriority verbosity;

public:

    void SetVerbosity(LogPriority new_priority);

    const char* GetVerbocityStr(LogPriority priority);

    void Log(LogPriority priority, std::string message);

    void LogDebug(std::string message);
    void LogError(std::string message);

};

#endif


