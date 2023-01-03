#include "../../src/logger/Logger.hpp"

void GameLogger::SetVerbosity(LogPriority new_priority)
{
    verbosity = new_priority;
}

const char* GameLogger::GetVerbocityStr(LogPriority priority)
{
    switch (priority) {
        case TraceP:
        {
            return "Trace";
        }
        case DebugP:
        {
            return "Debug";
        }
        case InfoP:
        {
            return "Info";
        }
        case WarnP:
        {
            return "Warning";
        }
        case ErrorP:
        {
            return "Error";
        }
        case FatalP:
        {
            return "Fatal";
        }
    }

    return "Failed verbosity";
}

void GameLogger::Log(LogPriority priority, std::string message)
{
    if (verbosity < 3 && priority < verbosity) return;

    printf(
        "%s: %s\n",
        GetVerbocityStr(priority),
        message.c_str()
    );
}

void GameLogger::LogDebug(std::string message)
{
    Log(LogPriority::DebugP, message);
}

void GameLogger::LogError(std::string message)
{
    Log(LogPriority::ErrorP, message);
}

