#include "ConfigLoader.h"
#include "../../Tests/SimpleTest.h"

TEST_CASE(ConfigLoader_AppliesWindowSettings) {
    Engine::ApplicationConfig config;
    std::string ini =
        "[window]\n"
        "title=Legal Crime Test\n"
        "width=1280\n"
        "height=720\n"
        "fullscreen=true\n"
        "resizable=true\n"
        "highdpi=false\n"
        "[renderer]\n"
        "vsync=true\n"
        "[app]\n"
        "name=Configured App\n"
        "loglevel=warning\n";

    auto result = Engine::ConfigLoader::ApplyIniString(ini, config);
    ASSERT_TRUE(result.success);
    ASSERT_STRING_EQUAL(config.windowConfig.title.c_str(), "Legal Crime Test");
    ASSERT_EQUAL(config.windowConfig.resolution.width, static_cast<uint32_t>(1280));
    ASSERT_EQUAL(config.windowConfig.resolution.height, static_cast<uint32_t>(720));
    ASSERT_TRUE(config.windowConfig.mode == Engine::WindowMode::Fullscreen);
    ASSERT_TRUE(config.windowConfig.resizable);
    ASSERT_FALSE(config.windowConfig.highDPI);
    ASSERT_TRUE(config.rendererConfig.vSync);
    ASSERT_STRING_EQUAL(config.appName.c_str(), "Configured App");
    ASSERT_TRUE(config.logLevel == Engine::LogLevel::Warning);
    return {"ConfigLoader_AppliesWindowSettings", true, ""};
}

TEST_CASE(ConfigLoader_UnknownKeysIgnored) {
    Engine::ApplicationConfig config;
    auto defaults = config;

    std::string ini =
        "[window]\n"
        "unknown_key=123\n"
        "[random]\n"
        "foo=bar\n";

    auto result = Engine::ConfigLoader::ApplyIniString(ini, config);
    ASSERT_TRUE(result.success);
    ASSERT_EQUAL(config.windowConfig.resolution.width, defaults.windowConfig.resolution.width);
    ASSERT_EQUAL(config.windowConfig.resolution.height, defaults.windowConfig.resolution.height);
    ASSERT_TRUE(config.logLevel == defaults.logLevel);
    return {"ConfigLoader_UnknownKeysIgnored", true, ""};
}

TEST_CASE(ConfigLoader_BoolParsingVariants) {
    Engine::ApplicationConfig config;
    std::string ini =
        "[window]\n"
        "fullscreen=yes\n"
        "resizable=on\n"
        "highdpi=0\n"
        "[renderer]\n"
        "vsync=1\n";

    auto result = Engine::ConfigLoader::ApplyIniString(ini, config);
    ASSERT_TRUE(result.success);
    ASSERT_TRUE(config.windowConfig.mode == Engine::WindowMode::Fullscreen);
    ASSERT_TRUE(config.windowConfig.resizable);
    ASSERT_FALSE(config.windowConfig.highDPI);
    ASSERT_TRUE(config.rendererConfig.vSync);
    return {"ConfigLoader_BoolParsingVariants", true, ""};
}
