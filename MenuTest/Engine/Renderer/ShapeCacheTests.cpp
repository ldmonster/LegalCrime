#include "ShapeCache.h"
#include "../../Tests/SimpleTest.h"

namespace {

class MockRenderer : public Engine::IRenderer {
public:
    Engine::Result<void> Initialize(Engine::IWindow*, const Engine::RendererConfig&) override {
        return Engine::Result<void>::Success();
    }
    void Shutdown() override {}
    void Clear(const Engine::Color&) override {}
    void Present() override {}
    SDL_Renderer* GetNativeRenderer() const override { return nullptr; }
    bool SetVSync(bool) override { return true; }
    bool IsVSyncEnabled() const override { return false; }
    bool IsInitialized() const override { return true; }

    void SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override {
        lastColor = {r, g, b, a};
        setDrawColorCalled = true;
    }

    void DrawLines(const Engine::Point* points, int count) override {
        drawLinesCalled = true;
        lastPointCount = count;
        if (points && count > 0) {
            lastFirstPoint = points[0];
        }
    }

    void DrawCircle(const Engine::Point&, int, int) override {}

    bool setDrawColorCalled = false;
    bool drawLinesCalled = false;
    int lastPointCount = 0;
    Engine::Point lastFirstPoint{0, 0};
    Engine::Color lastColor{0, 0, 0, 255};
};

} // namespace

TEST_CASE(ShapeCache_Construction_PrecomputesCorrectSegmentCount) {
    Engine::ShapeCache cache(16);
    ASSERT_EQUAL(cache.GetSegmentCount(), 16);
    return {"ShapeCache_Construction_PrecomputesCorrectSegmentCount", true, ""};
}

TEST_CASE(ShapeCache_Construction_DefaultIs32Segments) {
    Engine::ShapeCache cache;
    ASSERT_EQUAL(cache.GetSegmentCount(), 32);
    return {"ShapeCache_Construction_DefaultIs32Segments", true, ""};
}

TEST_CASE(ShapeCache_Construction_ClampsSegmentsBelow8) {
    Engine::ShapeCache cache(3);
    ASSERT_EQUAL(cache.GetSegmentCount(), 8);
    return {"ShapeCache_Construction_ClampsSegmentsBelow8", true, ""};
}

TEST_CASE(ShapeCache_DrawCircle_CallsSetDrawColorWithProvidedColor) {
    Engine::ShapeCache cache(32);
    MockRenderer renderer;

    cache.DrawCircle(renderer, {100, 100}, 50, Engine::Color(0, 255, 0, 255));

    ASSERT_TRUE(renderer.setDrawColorCalled);
    ASSERT_EQUAL(renderer.lastColor.r, 0);
    ASSERT_EQUAL(renderer.lastColor.g, 255);
    ASSERT_EQUAL(renderer.lastColor.b, 0);
    ASSERT_EQUAL(renderer.lastColor.a, 255);
    return {"ShapeCache_DrawCircle_CallsSetDrawColorWithProvidedColor", true, ""};
}

TEST_CASE(ShapeCache_DrawCircle_CallsDrawLinesWithCorrectPointCount) {
    Engine::ShapeCache cache(32);
    MockRenderer renderer;

    cache.DrawCircle(renderer, {0, 0}, 100, Engine::Color(255, 255, 255, 255));

    ASSERT_TRUE(renderer.drawLinesCalled);
    // segments + 1 points (closed loop)
    ASSERT_EQUAL(renderer.lastPointCount, 33);
    return {"ShapeCache_DrawCircle_CallsDrawLinesWithCorrectPointCount", true, ""};
}

TEST_CASE(ShapeCache_DrawCircle_ZeroRadiusDoesNotDraw) {
    Engine::ShapeCache cache(32);
    MockRenderer renderer;

    cache.DrawCircle(renderer, {50, 50}, 0, Engine::Color(255, 0, 0, 255));

    ASSERT_TRUE(!renderer.drawLinesCalled);
    return {"ShapeCache_DrawCircle_ZeroRadiusDoesNotDraw", true, ""};
}
