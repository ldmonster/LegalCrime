#include "ConfigLoader.h"
#include "FileSystem.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace Engine {
namespace {

    std::string Trim(const std::string& s) {
        size_t start = 0;
        while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
            ++start;
        }

        size_t end = s.size();
        while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
            --end;
        }

        return s.substr(start, end - start);
    }

    std::string Lower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return s;
    }

    bool ParseBool(const std::string& value) {
        std::string v = Lower(Trim(value));
        return v == "1" || v == "true" || v == "yes" || v == "on";
    }

    LogLevel ParseLogLevel(const std::string& value) {
        std::string v = Lower(Trim(value));
        if (v == "debug") return LogLevel::Debug;
        if (v == "warning") return LogLevel::Warning;
        if (v == "error") return LogLevel::Error;
        return LogLevel::Info;
    }

} // namespace

    ApplicationConfig ConfigLoader::LoadOrDefault(const std::string& path, const ApplicationConfig& defaults) {
        ApplicationConfig config = defaults;

        if (!FileSystem::Exists(path)) {
            return config;
        }

        std::ifstream in(path);
        if (!in.is_open()) {
            return config;
        }

        std::ostringstream buffer;
        buffer << in.rdbuf();
        auto parseResult = ApplyIniString(buffer.str(), config);
        if (!parseResult) {
            return defaults;
        }

        return config;
    }

    Result<void> ConfigLoader::ApplyIniString(const std::string& iniText, ApplicationConfig& config) {
        std::istringstream input(iniText);
        std::string section;
        std::string line;

        while (std::getline(input, line)) {
            std::string trimmed = Trim(line);
            if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == ';') {
                continue;
            }

            if (trimmed.front() == '[' && trimmed.back() == ']') {
                section = Lower(Trim(trimmed.substr(1, trimmed.size() - 2)));
                continue;
            }

            size_t eq = trimmed.find('=');
            if (eq == std::string::npos) {
                continue;
            }

            std::string key = Lower(Trim(trimmed.substr(0, eq)));
            std::string value = Trim(trimmed.substr(eq + 1));

            if (section == "window") {
                if (key == "title") {
                    config.windowConfig.title = value;
                } else if (key == "width") {
                    config.windowConfig.resolution.width = static_cast<uint32>(std::stoul(value));
                } else if (key == "height") {
                    config.windowConfig.resolution.height = static_cast<uint32>(std::stoul(value));
                } else if (key == "fullscreen") {
                    config.windowConfig.mode = ParseBool(value) ? WindowMode::Fullscreen : WindowMode::Windowed;
                } else if (key == "resizable") {
                    config.windowConfig.resizable = ParseBool(value);
                } else if (key == "vsync") {
                    config.windowConfig.vSync = ParseBool(value);
                } else if (key == "highdpi") {
                    config.windowConfig.highDPI = ParseBool(value);
                }
            } else if (section == "renderer") {
                if (key == "vsync") {
                    config.rendererConfig.vSync = ParseBool(value);
                }
            } else if (section == "app") {
                if (key == "name") {
                    config.appName = value;
                } else if (key == "loglevel") {
                    config.logLevel = ParseLogLevel(value);
                }
            }
        }

        return Result<void>::Success();
    }

} // namespace Engine
