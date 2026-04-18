#pragma once

#include <string>

namespace Engine {

    /// Cross-platform file path utilities.
    /// Uses SDL_GetBasePath() for executable-relative asset loading.
    /// All paths use '/' as separator (SDL handles conversion).
    class FileSystem {
    public:
        /// Join two path segments with the platform separator.
        static std::string JoinPath(const std::string& base, const std::string& relative);

        /// Get the directory containing the executable (cached).
        /// Falls back to "./" if SDL_GetBasePath() is unavailable.
        static const std::string& GetBasePath();

        /// Get the assets directory (base path by default).
        static std::string GetAssetsDir();

        /// Resolve a relative asset path to an absolute path.
        /// e.g. "Pics/capone.png" → "/path/to/exe/Pics/capone.png"
        static std::string ResolveAssetPath(const std::string& relativePath);

        /// Check if a file or directory exists.
        static bool Exists(const std::string& path);

        /// Normalize path separators to '/'.
        static std::string NormalizePath(const std::string& path);
    };

} // namespace Engine
