#pragma once

#include <memory>
#include <string>
#include <cstdint>

// Common type aliases for the engine
namespace Engine {
    
    // Smart pointer aliases
    template<typename T>
    using UniquePtr = std::unique_ptr<T>;
    
    template<typename T>
    using SharedPtr = std::shared_ptr<T>;
    
    template<typename T>
    using WeakPtr = std::weak_ptr<T>;
    
    // Integer types
    using int8 = int8_t;
    using int16 = int16_t;
    using int32 = int32_t;
    using int64 = int64_t;
    
    using uint8 = uint8_t;
    using uint16 = uint16_t;
    using uint32 = uint32_t;
    using uint64 = uint64_t;
    
    // Common structures
    struct Resolution {
        uint32 width;
        uint32 height;
        
        Resolution(uint32 w = 800, uint32 h = 600) : width(w), height(h) {}
    };
    
    struct Color {
        uint8 r, g, b, a;

        Color(uint8 red = 255, uint8 green = 255, uint8 blue = 255, uint8 alpha = 255)
            : r(red), g(green), b(blue), a(alpha) {}
    };

    struct Point {
        int x, y;

        Point(int xPos = 0, int yPos = 0) : x(xPos), y(yPos) {}
    };

    // Result type for operations that can fail
    template<typename T>
    struct Result {
        bool success;
        T value;
        std::string error;
        
        Result() : success(false), value{}, error("") {}
        Result(T val) : success(true), value(val), error("") {}
        Result(const std::string& err) : success(false), value{}, error(err) {}
        
        operator bool() const { return success; }
    };
    
    // Specialization for void operations
    template<>
    struct Result<void> {
        bool success;
        std::string error;
        
        Result() : success(true), error("") {}
        Result(bool s, const std::string& err = "") : success(s), error(err) {}
        
        operator bool() const { return success; }
        
        static Result Success() { return Result(true); }
        static Result Failure(const std::string& err) { return Result(false, err); }
    };
}
