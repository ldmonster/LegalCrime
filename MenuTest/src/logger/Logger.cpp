#define _CRT_SECURE_NO_WARNINGS
#include "../../src/logger/Logger.hpp"

#pragma warning(disable: 4996)

GameLogger::GameLogger() {
    verbosity = LogPriority::InfoP;
}

GameLogger::~GameLogger() = default;

void GameLogger::SetVerbosity(LogPriority new_priority) {
    const std::lock_guard<std::mutex> lock(log_mutex);
    verbosity = new_priority;
}

const char* GameLogger::GetVerbocityStr(LogPriority priority) noexcept {
    switch (priority) {
        case LogPriority::TraceP: return "Trace";
        case LogPriority::DebugP: return "Debug";
        case LogPriority::InfoP: return "Info";
        case LogPriority::WarnP: return "Warning";
        case LogPriority::ErrorP: return "Error";
        case LogPriority::FatalP: return "Fatal";
        default: return "Unknown";
    }
}

std::string GameLogger::getCurrentTime() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t time_now = std::chrono::system_clock::to_time_t(now);

    char buffer[64];
    std::tm time_struct;
    localtime_s(&time_struct, &time_now);
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &time_struct);

    return std::string(buffer);
}

void GameLogger::formatAndOutput(LogPriority priority, const std::string& message
#if __cplusplus >= 202002L
                                 , std::source_location location
#endif
                                 ) {
    std::ostream& output = (priority >= LogPriority::ErrorP) ? std::cerr : std::cout;

    std::ostringstream ss;
    ss << '[' << getCurrentTime() << "] " 
       << GetVerbocityStr(priority) << ": " 
       << message;

#if __cplusplus >= 202002L
    if (priority >= LogPriority::DebugP) {
        ss << " (" << location.file_name() << ':' << location.line() << ')';
    }
#endif

    output << ss.str() << '\n';
    output.flush();
}

void GameLogger::Log(LogPriority priority, const std::string& message) {
    const std::lock_guard<std::mutex> lock(log_mutex);

    if (priority < verbosity) {
        return;
    }

    formatAndOutput(priority, message);
}

#if __cplusplus >= 202002L
void GameLogger::Log(LogPriority priority, const std::string& message, std::source_location location) {
    const std::lock_guard<std::mutex> lock(log_mutex);

    if (priority < verbosity) {
        return;
    }

    formatAndOutput(priority, message, location);
}
#endif

void GameLogger::LogDebug(const std::string& message) {
    Log(LogPriority::DebugP, message);
}

void GameLogger::LogError(const std::string& message) {
    Log(LogPriority::ErrorP, message);
}

