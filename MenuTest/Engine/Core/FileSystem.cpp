#include "FileSystem.h"
#include <SDL3/SDL.h>
#include <algorithm>

namespace Engine {

    static std::string s_basePath;

    std::string FileSystem::JoinPath(const std::string& base, const std::string& relative) {
        if (base.empty()) return NormalizePath(relative);
        if (relative.empty()) return NormalizePath(base);

        char lastChar = base.back();
        bool baseHasSep = (lastChar == '/' || lastChar == '\\');
        bool relHasSep = (relative.front() == '/' || relative.front() == '\\');

        std::string result;
        if (baseHasSep && relHasSep) {
            result = base + relative.substr(1);
        } else if (!baseHasSep && !relHasSep) {
            result = base + "/" + relative;
        } else {
            result = base + relative;
        }

        return NormalizePath(result);
    }

    const std::string& FileSystem::GetBasePath() {
        if (s_basePath.empty()) {
            const char* sdlBase = SDL_GetBasePath();
            if (sdlBase) {
                s_basePath = sdlBase;
            } else {
                s_basePath = "./";
            }
        }
        return s_basePath;
    }

    std::string FileSystem::GetAssetsDir() {
        return GetBasePath();
    }

    std::string FileSystem::ResolveAssetPath(const std::string& relativePath) {
        return JoinPath(GetAssetsDir(), relativePath);
    }

    bool FileSystem::Exists(const std::string& path) {
        SDL_IOStream* stream = SDL_IOFromFile(path.c_str(), "rb");
        if (stream) {
            SDL_CloseIO(stream);
            return true;
        }
        return false;
    }

    std::string FileSystem::NormalizePath(const std::string& path) {
        std::string result = path;
        std::replace(result.begin(), result.end(), '\\', '/');
        return result;
    }

} // namespace Engine
