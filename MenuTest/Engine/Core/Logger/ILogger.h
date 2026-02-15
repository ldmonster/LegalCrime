#pragma once

#include "../Types.h"
#include <string>

namespace Engine {
    
    enum class LogLevel {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };
    
    // Abstract logger interface - allows for different logging implementations
    class ILogger {
    public:
        virtual ~ILogger() = default;
        
        virtual void SetLogLevel(LogLevel level) = 0;
        virtual void Log(LogLevel level, const std::string& message) = 0;
        
        // Convenience methods
        void Trace(const std::string& message) { Log(LogLevel::Trace, message); }
        void Debug(const std::string& message) { Log(LogLevel::Debug, message); }
        void Info(const std::string& message) { Log(LogLevel::Info, message); }
        void Warning(const std::string& message) { Log(LogLevel::Warning, message); }
        void Error(const std::string& message) { Log(LogLevel::Error, message); }
        void Fatal(const std::string& message) { Log(LogLevel::Fatal, message); }
    };
}
