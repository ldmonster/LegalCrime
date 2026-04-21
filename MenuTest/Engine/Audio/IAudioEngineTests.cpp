#include "IAudioEngine.h"
#include "IMusicPlayer.h"
#include "ISoundPlayer.h"
#include "../../Tests/SimpleTest.h"

namespace {

class FakeMusicPlayer : public Engine::IMusicPlayer {
public:
    Engine::Result<void> LoadMusicFromDirectory(const std::string&) override { return Engine::Result<void>::Success(); }
    Engine::Result<void> PlayMusic() override { return Engine::Result<void>::Success(); }
    Engine::Result<void> StopMusic() override { return Engine::Result<void>::Success(); }
    Engine::Result<void> PauseMusic() override { return Engine::Result<void>::Success(); }
    Engine::Result<void> ResumeMusic() override { return Engine::Result<void>::Success(); }
    void NextTrack() override {}
    void PreviousTrack() override {}
    void RandomTrack() override {}
    bool IsPlaying() const override { return false; }
};

class FakeSoundPlayer : public Engine::ISoundPlayer {
public:
    std::shared_ptr<Engine::SoundEffect> LoadSoundEffect(const std::string&) override { return nullptr; }
    Engine::Result<void> PlaySoundEffect(const std::shared_ptr<Engine::SoundEffect>&, int) override {
        return Engine::Result<void>::Success();
    }
};

class FakeAudioEngine : public Engine::IAudioEngine {
public:
    Engine::Result<void> Initialize() override { initialized = true; return Engine::Result<void>::Success(); }
    void Shutdown() override { initialized = false; }
    void SetVolume(float volume) override { m_volume = volume; }
    float GetVolume() const override { return m_volume; }
    bool IsInitialized() const override { return initialized; }
    Engine::IMusicPlayer& GetMusicPlayer() override { return music; }
    Engine::ISoundPlayer& GetSoundPlayer() override { return sound; }

private:
    bool initialized = false;
    float m_volume = 1.0f;
    FakeMusicPlayer music;
    FakeSoundPlayer sound;
};

} // namespace

TEST_CASE(IAudioEngine_ProvidesFocusedInterfaces) {
    FakeAudioEngine engine;
    auto& music = engine.GetMusicPlayer();
    auto& sound = engine.GetSoundPlayer();

    auto loadResult = music.LoadMusicFromDirectory(".");
    auto soundResult = sound.PlaySoundEffect(nullptr, 0);

    ASSERT_TRUE(loadResult.success);
    ASSERT_TRUE(soundResult.success);
    return {"IAudioEngine_ProvidesFocusedInterfaces", true, ""};
}

TEST_CASE(IAudioEngine_LifecycleStillAvailable) {
    FakeAudioEngine engine;
    ASSERT_FALSE(engine.IsInitialized());
    ASSERT_TRUE(engine.Initialize().success);
    ASSERT_TRUE(engine.IsInitialized());
    engine.Shutdown();
    ASSERT_FALSE(engine.IsInitialized());
    return {"IAudioEngine_LifecycleStillAvailable", true, ""};
}
