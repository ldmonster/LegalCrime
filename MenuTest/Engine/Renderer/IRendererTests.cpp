#include "IRenderer.h"
#include "../../Tests/SimpleTest.h"

namespace {

class MockRenderer : public Engine::IRenderer {
public:
    Engine::Result<void> Initialize(Engine::IWindow*, const Engine::RendererConfig&) override {
        initialized = true;
        return Engine::Result<void>::Success();
    }

    void Shutdown() override { initialized = false; }
    void Clear(const Engine::Color&) override {}
    void Present() override {}
    SDL_Renderer* GetNativeRenderer() const override { return nullptr; }
    bool SetVSync(bool enabled) override { vSync = enabled; return true; }
    bool IsVSyncEnabled() const override { return vSync; }
    bool IsInitialized() const override { return initialized; }

    void SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override {
        color = {r, g, b, a};
    }

    void DrawLines(const Engine::Point*, int) override {}

    void DrawCircle(const Engine::Point& center, int radius, int segments = 32) override {
        lastCenter = center;
        lastRadius = radius;
        lastSegments = segments;
        drawCircleCalled = true;
    }

    bool drawCircleCalled = false;
    Engine::Point lastCenter{0, 0};
    int lastRadius = 0;
    int lastSegments = 0;

private:
    bool initialized = false;
    bool vSync = false;
    Engine::Color color{0, 0, 0, 255};
};

} // namespace

TEST_CASE(IRenderer_DrawCircle_Available) {
    MockRenderer renderer;
    renderer.DrawCircle(Engine::Point(10, 20), 30, 24);

    ASSERT_TRUE(renderer.drawCircleCalled);
    ASSERT_EQUAL(renderer.lastCenter.x, 10);
    ASSERT_EQUAL(renderer.lastCenter.y, 20);
    ASSERT_EQUAL(renderer.lastRadius, 30);
    ASSERT_EQUAL(renderer.lastSegments, 24);
    return {"IRenderer_DrawCircle_Available", true, ""};
}
