#include "FlxCamera.h"
#include "FlxG.h"
#include "FlxObject.h"
#include "FlxSprite.h"
#include "math/FlxMath.h"
#include "math/FlxMatrix.h"
#include "math/FlxPoint.h"
#include "math/FlxRect.h"
#include "util/FlxAxes.h"
#include "util/FlxColor.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

namespace flixel {

float FlxCamera::defaultZoom = 1.0f;
std::vector<FlxCamera*> FlxCamera::_defaultCameras;

FlxCamera::FlxCamera(float x, float y, int width, int height, float zoom)
    : FlxBasic()
    , x(x)
    , y(y)
    , width(width)
    , height(height)
    , zoom(zoom)
    , renderer(FlxG::renderer)
{
    scaleX = 1.0f;
    scaleY = 1.0f;
    totalScaleX = 1.0f;
    totalScaleY = 1.0f;
    style = FlxCameraFollowStyle::LOCKON;
    followLerp = 1.0f;
    scroll = FlxPoint(0, 0);
    targetOffset = FlxPoint(0, 0);
    followLead = FlxPoint(0, 0);
    _lastTargetPosition = FlxPoint(0, 0);
    _scrollTarget = FlxPoint(0, 0);
    _flashOffset = FlxPoint(0, 0);
    _point = FlxPoint(0, 0);
    _helperPoint = FlxPoint(0, 0);
    _bounds = math::FlxRect(0, 0, 0, 0);
    _helperMatrix = math::FlxMatrix();
    _blitMatrix = math::FlxMatrix();

    _fill = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1, 1);
    if (_fill) {
        SDL_SetTextureBlendMode(_fill, SDL_BLENDMODE_BLEND);
    }

    if (zoom == 0) {
        zoom = defaultZoom;
    }

    if (width <= 0) {
        width = static_cast<int>(std::ceil(FlxG::width / zoom));
    }
    if (height <= 0) {
        height = static_cast<int>(std::ceil(FlxG::height / zoom));
    }

    this->width = width;
    this->height = height;
    _flashRect = {0, 0, width, height};
    _flashPoint = {0, 0};

    pixelPerfectRender = false; // todo: perfect my pixels...

    buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

    color = util::FlxColor::WHITE;
    this->zoom = initialZoom = zoom;

    updateScrollRect();
    updateFlashOffset();
    updateFlashSpritePosition();
    updateInternalSpritePositions();

    bgColor = util::FlxColor::BLACK; // todo: git from FlxG::cameras
}

FlxCamera::~FlxCamera() {
    destroy();
}

void FlxCamera::destroy() {
    if (_fill) {
        SDL_DestroyTexture(_fill);
        _fill = nullptr;
    }
    if (buffer) {
        SDL_DestroyTexture(buffer);
        buffer = nullptr;
    }
    for (auto* filter : filters) {
        if (filter) {
            SDL_DestroyTexture(filter);
        }
    }
    filters.clear();
    FlxBasic::destroy();

    if (deadzone) {
        delete deadzone;
        deadzone = nullptr;
    }

    target = nullptr;
}

void FlxCamera::update(float elapsed) {
    if (target != nullptr) {
        updateFollow();
        updateLerp(elapsed);
    }

    updateScroll();
    updateFlash(elapsed);
    updateFade(elapsed);

    if (filtersEnabled && !filters.empty()) {
        // todo: filter shit lmao!
    }

    updateFlashSpritePosition();
    updateShake(elapsed);
}

void FlxCamera::updateScroll() {
    bindScrollPos(scroll);
}

void FlxCamera::bindScrollPos(FlxPoint& scrollPos) {
    float minX = (minScrollX == 0) ? 0 : minScrollX - getViewMarginLeft();
    float maxX = (maxScrollX == 0) ? 0 : maxScrollX - getViewMarginRight();
    float minY = (minScrollY == 0) ? 0 : minScrollY - getViewMarginTop();
    float maxY = (maxScrollY == 0) ? 0 : maxScrollY - getViewMarginBottom();

    scrollPos.x = std::clamp(scrollPos.x, minX, maxX);
    scrollPos.y = std::clamp(scrollPos.y, minY, maxY);
}

void FlxCamera::updateFollow() {
    if (deadzone == nullptr) {
        float targetX = target->x + target->width * 0.5f;
        float targetY = target->y + target->height * 0.5f;
        _point.set(targetX, targetY);
        _point.add(targetOffset.x, targetOffset.y);
        _scrollTarget.set(_point.x - width * 0.5f, _point.y - height * 0.5f);
    } else {
        float edge;
        float targetX = target->x + targetOffset.x;
        float targetY = target->y + targetOffset.y;

        if (style == FlxCameraFollowStyle::SCREEN_BY_SCREEN) {
            if (targetX >= getViewRight()) {
                _scrollTarget.x += getViewWidth();
            } else if (targetX + target->width < getViewLeft()) {
                _scrollTarget.x -= getViewWidth();
            }

            if (targetY >= getViewBottom()) {
                _scrollTarget.y += getViewHeight();
            } else if (targetY + target->height < getViewTop()) {
                _scrollTarget.y -= getViewHeight();
            }

            bindScrollPos(_scrollTarget);
        } else {
            edge = targetX - deadzone->x;
            if (_scrollTarget.x > edge) {
                _scrollTarget.x = edge;
            }
            edge = targetX + target->width - deadzone->x - deadzone->width;
            if (_scrollTarget.x < edge) {
                _scrollTarget.x = edge;
            }

            edge = targetY - deadzone->y;
            if (_scrollTarget.y > edge) {
                _scrollTarget.y = edge;
            }
            edge = targetY + target->height - deadzone->y - deadzone->height;
            if (_scrollTarget.y < edge) {
                _scrollTarget.y = edge;
            }
        }

        if (dynamic_cast<FlxSprite*>(target) != nullptr) {
            if (_lastTargetPosition.x == 0 && _lastTargetPosition.y == 0) {
                _lastTargetPosition.set(target->x, target->y);
            }
            _scrollTarget.x += (target->x - _lastTargetPosition.x) * followLead.x;
            _scrollTarget.y += (target->y - _lastTargetPosition.y) * followLead.y;

            _lastTargetPosition.x = target->x;
            _lastTargetPosition.y = target->y;
        }
    }
}

void FlxCamera::updateLerp(float elapsed) {
    if (followLerp >= 1.0f) {
        scroll = _scrollTarget;
    } else if (followLerp > 0.0f) {
        float adjustedLerp = 1.0f - std::pow(1.0f - followLerp, elapsed * 60.0f);
        scroll.x += (_scrollTarget.x - scroll.x) * adjustedLerp;
        scroll.y += (_scrollTarget.y - scroll.y) * adjustedLerp;
    }
}

void FlxCamera::follow(FlxObject* target, FlxCameraFollowStyle style, float lerp) {
    this->style = style;
    this->target = target;
    followLerp = lerp;
    _lastTargetPosition.set(0, 0);
    if (deadzone) {
        delete deadzone;
        deadzone = nullptr;
    }

    switch (style) {
        case FlxCameraFollowStyle::LOCKON: {
            float w = (target != nullptr) ? target->width : 0;
            float h = (target != nullptr) ? target->height : 0;
            deadzone = new math::FlxRect((width - w) / 2, (height - h) / 2 - h * 0.25f, w, h);
            break;
        }
        case FlxCameraFollowStyle::PLATFORMER: {
            float w = width / 8;
            float h = height / 3;
            deadzone = new math::FlxRect((width - w) / 2, (height - h) / 2 - h * 0.25f, w, h);
            break;
        }
        case FlxCameraFollowStyle::TOPDOWN: {
            float helper = std::max(width, height) / 4;
            deadzone = new math::FlxRect((width - helper) / 2, (height - helper) / 2, helper, helper);
            break;
        }
        case FlxCameraFollowStyle::TOPDOWN_TIGHT: {
            float helper = std::max(width, height) / 8;
            deadzone = new math::FlxRect((width - helper) / 2, (height - helper) / 2, helper, helper);
            break;
        }
        case FlxCameraFollowStyle::SCREEN_BY_SCREEN:
            deadzone = new math::FlxRect(0, 0, width, height);
            break;
        case FlxCameraFollowStyle::NO_DEAD_ZONE:
            deadzone = nullptr;
            break;
    }
}

void FlxCamera::updateFlash(float elapsed) {
    if (_fxFlashAlpha > 0.0f) {
        _fxFlashAlpha -= elapsed / _fxFlashDuration;
        if (_fxFlashAlpha <= 0.0f && _fxFlashComplete) {
            _fxFlashComplete();
        }
    }
}

void FlxCamera::updateFade(float elapsed) {
    if (_fxFadeDuration == 0.0f) {
        return;
    }

    if (_fxFadeIn) {
        _fxFadeAlpha -= elapsed / _fxFadeDuration;
        if (_fxFadeAlpha <= 0.0f) {
            _fxFadeAlpha = 0.0f;
            completeFade();
        }
    } else {
        _fxFadeAlpha += elapsed / _fxFadeDuration;
        if (_fxFadeAlpha >= 1.0f) {
            _fxFadeAlpha = 1.0f;
            completeFade();
        }
    }
}

void FlxCamera::updateShake(float elapsed) {
    if (_fxShakeDuration > 0.0f) {
        _fxShakeDuration -= elapsed;
        if (_fxShakeDuration <= 0.0f) {
            if (_fxShakeComplete) {
                _fxShakeComplete();
            }
        } else {
            bool pixelPerfect = (pixelPerfectShake) ? pixelPerfectShake : pixelPerfectRender;
            
            if (static_cast<int>(_fxShakeAxes) & static_cast<int>(util::FlxAxes::X)) {
                float shakePixels = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * _fxShakeIntensity * width;
                if (pixelPerfect) {
                    shakePixels = std::round(shakePixels);
                }
                x += shakePixels * zoom;
            }
            
            if (static_cast<int>(_fxShakeAxes) & static_cast<int>(util::FlxAxes::Y)) {
                float shakePixels = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * _fxShakeIntensity * height;
                if (pixelPerfect) {
                    shakePixels = std::round(shakePixels);
                }
                y += shakePixels * zoom;
            }
        }
    }
}

void FlxCamera::updateFlashSpritePosition() {
    x = x + _flashOffset.x;
    y = y + _flashOffset.y;
}

void FlxCamera::updateFlashOffset() {
    _flashOffset.x = width * 0.5f * initialZoom;
    _flashOffset.y = height * 0.5f * initialZoom;
}

void FlxCamera::updateScrollRect() {
    _flashRect.x = 0;
    _flashRect.y = 0;
    _flashRect.w = static_cast<int>(width * initialZoom);
    _flashRect.h = static_cast<int>(height * initialZoom);
}

void FlxCamera::updateInternalSpritePositions() {}

void FlxCamera::updateBlitMatrix() {
    _blitMatrix.identity();
    _blitMatrix.tx = -getViewMarginLeft();
    _blitMatrix.ty = -getViewMarginTop();
    _blitMatrix.a = scaleX;
    _blitMatrix.d = scaleY;
    _useBlitMatrix = (scaleX < initialZoom) || (scaleY < initialZoom);
}

void FlxCamera::snapToTarget() {
    updateFollow();
    scroll = _scrollTarget;
}

void FlxCamera::focusOn(const FlxPoint& point) {
    scroll.set(point.x - width * 0.5f, point.y - height * 0.5f);
}

void FlxCamera::flash(util::FlxColor color, float duration, std::function<void()> onComplete, bool force) {
    if (!force && _fxFlashAlpha > 0.0f) {
        return;
    }

    _fxFlashColor = color;
    if (duration <= 0.0f) {
        duration = 0.000001f;
    }
    _fxFlashDuration = duration;
    _fxFlashComplete = onComplete;
    _fxFlashAlpha = 1.0f;
}

void FlxCamera::fade(util::FlxColor color, float duration, bool fadeIn, std::function<void()> onComplete, bool force) {
    if (_fxFadeDuration > 0.0f && !force) {
        return;
    }

    _fxFadeColor = color;
    if (duration <= 0.0f) {
        duration = 0.000001f;
    }

    _fxFadeIn = fadeIn;
    _fxFadeDuration = duration;
    _fxFadeComplete = onComplete;
    _fxFadeAlpha = _fxFadeIn ? 0.999999f : 0.000001f;
}

void FlxCamera::shake(float intensity, float duration, std::function<void()> onComplete, bool force, util::FlxAxes axes) {
    if (!force && _fxShakeDuration > 0.0f) {
        return;
    }

    _fxShakeIntensity = intensity;
    _fxShakeDuration = duration;
    _fxShakeComplete = onComplete;
    _fxShakeAxes = axes;
}

void FlxCamera::stopFade() {
    _fxFadeAlpha = 0.0f;
    _fxFadeDuration = 0.0f;
}

void FlxCamera::stopFlash() {
    _fxFlashAlpha = 0.0f;
    updateFlashSpritePosition();
}

void FlxCamera::stopShake() {
    _fxShakeDuration = 0.0f;
}

void FlxCamera::stopFX() {
    _fxFadeAlpha = 0.0f;
    _fxFadeDuration = 0.0f;
    _fxFlashAlpha = 0.0f;
    updateFlashSpritePosition();
    _fxShakeDuration = 0.0f;
}

FlxCamera& FlxCamera::copyFrom(const FlxCamera& camera) {
    setScrollBounds(camera.minScrollX, camera.maxScrollX, camera.minScrollY, camera.maxScrollY);
    target = camera.target;

    if (target != nullptr) {
        if (camera.deadzone == nullptr) {
            deadzone = nullptr;
        } else {
            if (deadzone == nullptr) {
                deadzone = new math::FlxRect();
            }
            *deadzone = *camera.deadzone;
        }
    }
    return *this;
}

void FlxCamera::fill(util::FlxColor color, bool blendAlpha, float fxAlpha) {
    if (!renderer || !_fill) return;

    SDL_SetTextureColorMod(_fill, color.red(), color.green(), color.blue());
    
    if (blendAlpha) {
        SDL_SetTextureAlphaMod(_fill, static_cast<Uint8>(color.alpha() * fxAlpha * 255));
        SDL_SetTextureBlendMode(_fill, SDL_BLENDMODE_BLEND);
    } else {
        SDL_SetTextureAlphaMod(_fill, 255);
        SDL_SetTextureBlendMode(_fill, SDL_BLENDMODE_NONE);
    }

    SDL_Rect dest = {
        static_cast<int>(x),
        static_cast<int>(y),
        width,
        height
    };

    SDL_RenderCopy(renderer, _fill, nullptr, &dest);
}

void FlxCamera::drawFX() {
    if (_fxFlashAlpha > 0.0f) {
        util::FlxColor flashColor = _fxFlashColor;
        Uint8 alpha = static_cast<Uint8>(flashColor.alpha() * _fxFlashAlpha);
        flashColor.setAlpha(alpha);
        fill(flashColor);
    }

    if (_fxFadeAlpha > 0.0f) {
        util::FlxColor fadeColor = _fxFadeColor;
        Uint8 alpha = static_cast<Uint8>(fadeColor.alpha() * _fxFadeAlpha);
        fadeColor.setAlpha(alpha);
        fill(fadeColor);
    }
}

void FlxCamera::checkResize() {
    if (width != _flashRect.w || height != _flashRect.h) {
        SDL_DestroyTexture(buffer);
        buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
        _flashRect.w = width;
        _flashRect.h = height;
        SDL_DestroyTexture(_fill);
        _fill = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
        updateBlitMatrix();
    }
}

void FlxCamera::setSize(int width, int height) {
    this->width = width;
    this->height = height;
}

void FlxCamera::setPosition(float x, float y) {
    this->x = x;
    this->y = y;
}

void FlxCamera::setScrollBoundsRect(float x, float y, float width, float height, bool updateWorld) {
    if (updateWorld) {
        FlxG::worldBounds.x = x;
        FlxG::worldBounds.y = y;
        FlxG::worldBounds.width = width;
        FlxG::worldBounds.height = height;
    }
    setScrollBounds(x, x + width, y, y + height);
}

void FlxCamera::setScrollBounds(float minX, float maxX, float minY, float maxY) {
    minScrollX = minX;
    maxScrollX = maxX;
    minScrollY = minY;
    maxScrollY = maxY;
    updateScroll();
}

void FlxCamera::setScale(float x, float y) {
    scaleX = x;
    scaleY = y;

    totalScaleX = scaleX;
    totalScaleY = scaleY;

    updateBlitMatrix();
    calcMarginX();
    calcMarginY();

    updateScrollRect();
    updateInternalSpritePositions();
}

void FlxCamera::onResize() {
    updateFlashOffset();
    setScale(scaleX, scaleY);
}

math::FlxRect FlxCamera::getViewMarginRect(math::FlxRect* rect) {
    if (rect == nullptr) {
        rect = new math::FlxRect();
    }
    rect->x = getViewMarginLeft();
    rect->y = getViewMarginTop();
    rect->width = getViewWidth();
    rect->height = getViewHeight();
    return *rect;
}

bool FlxCamera::containsPoint(const FlxPoint& point, float width, float height) {
    bool contained = (point.x + width > getViewMarginLeft()) && (point.x < getViewMarginRight()) &&
                    (point.y + height > getViewMarginTop()) && (point.y < getViewMarginBottom());
    return contained;
}

bool FlxCamera::containsRect(const math::FlxRect& rect) {
    bool contained = (rect.x + rect.width > getViewMarginLeft()) && (rect.x < getViewMarginRight()) &&
                    (rect.y + rect.height > getViewMarginTop()) && (rect.y < getViewMarginBottom());
    return contained;
}

void FlxCamera::calcMarginX() {
    viewMarginX = 0.5f * width * (scaleX - initialZoom) / scaleX;
}

void FlxCamera::calcMarginY() {
    viewMarginY = 0.5f * height * (scaleY - initialZoom) / scaleY;
}

void FlxCamera::completeFade() {
    _fxFadeDuration = 0.0f;
    if (_fxFadeComplete) {
        _fxFadeComplete();
    }
}
}