#pragma once

#include "FlxBasic.h"
#include "math/FlxPoint.h"
#include "math/FlxRect.h"
#include "math/FlxMatrix.h"
#include "util/FlxAxes.h"
#include "util/FlxColor.h"
#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include <functional>

namespace flixel {

class FlxObject;
class FlxSprite;
class FlxGraphic;
class FlxFrame;
class FlxShader;

enum class FlxCameraFollowStyle {
    LOCKON,
    PLATFORMER,
    TOPDOWN,
    TOPDOWN_TIGHT,
    SCREEN_BY_SCREEN,
    NO_DEAD_ZONE
};

class FlxCamera : public FlxBasic {
public:
    static float defaultZoom;
    static std::vector<FlxCamera*> _defaultCameras;

    float x = 0;
    float y = 0;
    float scaleX = 0;
    float scaleY = 0;
    float totalScaleX = 0;
    float totalScaleY = 0;
    FlxCameraFollowStyle style;
    flixel::FlxObject* target = nullptr;
    flixel::FlxPoint targetOffset;
    float followLerp = 1.0f;
    flixel::math::FlxRect* deadzone = nullptr;
    float minScrollX = 0;
    float maxScrollX = 0;
    float minScrollY = 0;
    float maxScrollY = 0;
    flixel::FlxPoint scroll;
    flixel::util::FlxColor bgColor;
    bool useBgAlphaBlending = false;
    bool pixelPerfectRender = false;
    bool pixelPerfectShake = false;
    int width = 0;
    int height = 0;
    float zoom = 0;
    float viewMarginX = 0;
    float viewMarginY = 0;
    float initialZoom = 1.0f;
    float alpha = 1.0f;
    float angle = 0.0f;
    flixel::util::FlxColor color = flixel::util::FlxColor::WHITE;
    bool antialiasing = false;
    flixel::FlxPoint followLead;
    bool filtersEnabled = true;
    std::vector<SDL_Texture*> filters;

    FlxCamera(float x = 0.0f, float y = 0.0f, int width = 0, int height = 0, float zoom = 0.0f);
    ~FlxCamera();

    void destroy() override;
    void update(float elapsed) override;
    void updateScroll();
    void bindScrollPos(FlxPoint& scrollPos);
    void updateFollow();
    void updateLerp(float elapsed);
    void updateFlash(float elapsed);
    void updateFade(float elapsed);
    void updateShake(float elapsed);
    void updateFlashSpritePosition();
    void updateFlashOffset();
    void updateScrollRect();
    void updateInternalSpritePositions();
    void updateBlitMatrix();

    void follow(flixel::FlxObject* target, FlxCameraFollowStyle style = FlxCameraFollowStyle::LOCKON, float lerp = 1.0f);
    void snapToTarget();
    void focusOn(const FlxPoint& point);
    void flash(flixel::util::FlxColor color = flixel::util::FlxColor::WHITE, float duration = 1.0f, std::function<void()> onComplete = nullptr, bool force = false);
    void fade(flixel::util::FlxColor color = flixel::util::FlxColor::BLACK, float duration = 1.0f, bool fadeIn = false, std::function<void()> onComplete = nullptr, bool force = false);
    void shake(float intensity = 0.05f, float duration = 0.5f, std::function<void()> onComplete = nullptr, bool force = true, flixel::util::FlxAxes axes = flixel::util::FlxAxes::XY);
    void stopFade();
    void stopFlash();
    void stopShake();
    void stopFX();
    FlxCamera& copyFrom(const FlxCamera& camera);
    void fill(util::FlxColor color, bool blendAlpha = true, float fxAlpha = 1.0f);
    void drawFX();
    void checkResize();
    void setSize(int width, int height);
    void setPosition(float x = 0.0f, float y = 0.0f);
    void setScrollBoundsRect(float x = 0.0f, float y = 0.0f, float width = 0.0f, float height = 0.0f, bool updateWorld = false);
    void setScrollBounds(float minX, float maxX, float minY, float maxY);
    void setScale(float x, float y);
    void onResize();
    flixel::math::FlxRect getViewMarginRect(flixel::math::FlxRect* rect = nullptr);
    bool containsPoint(const FlxPoint& point, float width, float height);
    bool containsRect(const flixel::math::FlxRect& rect);

    float getViewMarginLeft() const { return viewMarginX; }
    float getViewMarginTop() const { return viewMarginY; }
    float getViewMarginRight() const { return width - viewMarginX; }
    float getViewMarginBottom() const { return height - viewMarginY; }
    float getViewWidth() const { return width - viewMarginX * 2; }
    float getViewHeight() const { return height - viewMarginY * 2; }
    float getViewX() const { return scroll.x + viewMarginX; }
    float getViewY() const { return scroll.y + viewMarginY; }
    float getViewLeft() const { return getViewX(); }
    float getViewTop() const { return getViewY(); }
    float getViewRight() const { return scroll.x + getViewMarginRight(); }
    float getViewBottom() const { return scroll.y + getViewMarginBottom(); }

private:
    flixel::math::FlxMatrix _blitMatrix;
    bool _useBlitMatrix = false;
    flixel::FlxPoint _lastTargetPosition;
    flixel::FlxPoint _scrollTarget;
    flixel::FlxPoint _flashOffset;
    flixel::FlxPoint _point;
    flixel::math::FlxRect _bounds;
    flixel::math::FlxMatrix _helperMatrix;
    flixel::FlxPoint _helperPoint;

    flixel::util::FlxColor _fxFlashColor = flixel::util::FlxColor::TRANSPARENT;
    float _fxFlashDuration = 0.0f;
    std::function<void()> _fxFlashComplete = nullptr;
    float _fxFlashAlpha = 0.0f;
    flixel::util::FlxColor _fxFadeColor = flixel::util::FlxColor::TRANSPARENT;
    float _fxFadeDuration = 0.0f;
    bool _fxFadeIn = false;
    std::function<void()> _fxFadeComplete = nullptr;
    float _fxFadeAlpha = 0.0f;
    float _fxShakeIntensity = 0.0f;
    float _fxShakeDuration = 0.0f;
    std::function<void()> _fxShakeComplete = nullptr;
    flixel::util::FlxAxes _fxShakeAxes = flixel::util::FlxAxes::XY;

    SDL_Texture* buffer = nullptr;
    SDL_Texture* _fill = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Rect _flashRect;
    SDL_Point _flashPoint;

    void calcMarginX();
    void calcMarginY();
    void completeFade();
};
}