#pragma once

#include "Application.h"
#include <string>

namespace Engine {

    // Minimal INI-based application config loader.
    class ConfigLoader {
    public:
        // Load from a file path if it exists; otherwise return defaults.
        static ApplicationConfig LoadOrDefault(const std::string& path, const ApplicationConfig& defaults);

        // Parse INI text and apply known keys onto config.
        static Result<void> ApplyIniString(const std::string& iniText, ApplicationConfig& config);
    };

} // namespace Engine
