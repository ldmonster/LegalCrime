#include "Logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

namespace Engine {
    
    Logger::Logger() 
        : m_minLevel(LogLevel::Info)
        , m_fileStream(nullptr) {
    }
    
    Logger::~Logger() {
        if (m_fileStream && m_fileStream->is_open()) {
            m_fileStream->close();
        }
    }
    
    void Logger::SetLogLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_minLevel = level;
    }
    
    void Logger::Log(LogLevel level, const std::string& message) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (level < m_minLevel) {
            return;
        }
        
        OutputMessage(level, message);
    }
    
    void Logger::EnableFileLogging(const std::string& filename) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        m_fileStream = std::make_unique<std::ofstream>(filename, std::ios::app);
        if (!m_fileStream->is_open()) {
            std::cerr << "Failed to open log file: " << filename << std::endl;
            m_fileStream.reset();
        }
    }
    
    void Logger::DisableFileLogging() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_fileStream && m_fileStream->is_open()) {
            m_fileStream->close();
        }
        m_fileStream.reset();
    }
    
    const char* Logger::GetLevelString(LogLevel level) const {
        switch (level) {
            case LogLevel::Trace:   return "TRACE";
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error:   return "ERROR";
            case LogLevel::Fatal:   return "FATAL";
            default:                return "UNKNOWN";
        }
    }
    
    std::string Logger::GetTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::tm timeStruct;
#ifdef _WIN32
        localtime_s(&timeStruct, &time);
#else
        localtime_r(&time, &timeStruct);
#endif
        
        std::ostringstream ss;
        ss << std::put_time(&timeStruct, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void Logger::OutputMessage(LogLevel level, const std::string& message) {
        std::ostringstream ss;
        ss << "[" << GetTimestamp() << "] "
           << "[" << GetLevelString(level) << "] "
           << message;
        
        std::string formattedMessage = ss.str();
        
        // Console output
        std::ostream& output = (level >= LogLevel::Error) ? std::cerr : std::cout;
        output << formattedMessage << std::endl;
        
        // File output
        if (m_fileStream && m_fileStream->is_open()) {
            (*m_fileStream) << formattedMessage << std::endl;
            m_fileStream->flush();
        }
    }
}
