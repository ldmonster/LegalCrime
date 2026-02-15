#pragma once

#include "ILogger.h"
#include <mutex>
#include <fstream>
#include <memory>

namespace Engine {
    
    class Logger : public ILogger {
    public:
        Logger();
        ~Logger() override;
        
        void SetLogLevel(LogLevel level) override;
        void Log(LogLevel level, const std::string& message) override;
        
        // Optional: Enable file logging
        void EnableFileLogging(const std::string& filename);
        void DisableFileLogging();
        
    private:
        LogLevel m_minLevel;
        std::mutex m_mutex;
        std::unique_ptr<std::ofstream> m_fileStream;
        
        const char* GetLevelString(LogLevel level) const;
        std::string GetTimestamp() const;
        void OutputMessage(LogLevel level, const std::string& message);
    };
}
