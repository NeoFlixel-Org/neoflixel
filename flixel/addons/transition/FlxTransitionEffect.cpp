#include "FlxTransitionEffect.h"
#include "../../FlxG.h"
#include <SDL.h>
#include <cmath>

namespace flixel {
namespace addons {
namespace transition {

FlxTransitionEffect::FlxTransitionEffect(const TransitionData& data)
    : _data(data)
{
    _back = new FlxSprite(0, 0);
    
    float dirX = _data.direction.x;
    float dirY = _data.direction.y;
    
    int screenWidth = FlxG::width;
    int screenHeight = FlxG::height;
    
    if (dirX == 0 && dirY == 0) {
        SDL_Color sdlColor = {
            static_cast<Uint8>(_data.color.red()),
            static_cast<Uint8>(_data.color.green()),
            static_cast<Uint8>(_data.color.blue()),
            static_cast<Uint8>(_data.color.alpha())
        };
        _back->makeGraphic(screenWidth, screenHeight, sdlColor);
    } else if (dirX == 0 && dirY != 0) {
        createVerticalGradient(screenWidth, screenHeight, dirY > 0);
    } else if (dirX != 0 && dirY == 0) {
        createHorizontalGradient(screenWidth, screenHeight, dirX > 0);
    } else {
        SDL_Color sdlColor = {
            static_cast<Uint8>(_data.color.red()),
            static_cast<Uint8>(_data.color.green()),
            static_cast<Uint8>(_data.color.blue()),
            static_cast<Uint8>(_data.color.alpha())
        };
        _back->makeGraphic(static_cast<int>(screenWidth * 2.5f), screenHeight, sdlColor);
    }
    
    _back->scrollFactor.x = 0.0f;
    _back->scrollFactor.y = 0.0f;
}

void FlxTransitionEffect::createVerticalGradient(int width, int height, bool fromTop) {
    int totalHeight = height * 2;
    
    SDL_Texture* tex = SDL_CreateTexture(FlxG::renderer, SDL_PIXELFORMAT_RGBA8888, 
                                          SDL_TEXTUREACCESS_TARGET, width, totalHeight);
    if (!tex) return;
    
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(FlxG::renderer, tex);
    
    Uint8 r = static_cast<Uint8>(_data.color.red());
    Uint8 g = static_cast<Uint8>(_data.color.green());
    Uint8 b = static_cast<Uint8>(_data.color.blue());
    
    if (fromTop) {
        SDL_SetRenderDrawColor(FlxG::renderer, r, g, b, 255);
        SDL_Rect solidRect = {0, 0, width, height};
        SDL_RenderFillRect(FlxG::renderer, &solidRect);
        
        for (int y = 0; y < height; y++) {
            float alpha = 1.0f - (static_cast<float>(y) / height);
            SDL_SetRenderDrawColor(FlxG::renderer, r, g, b, static_cast<Uint8>(alpha * 255));
            SDL_RenderDrawLine(FlxG::renderer, 0, height + y, width, height + y);
        }
    } else {
        for (int y = 0; y < height; y++) {
            float alpha = static_cast<float>(y) / height;
            SDL_SetRenderDrawColor(FlxG::renderer, r, g, b, static_cast<Uint8>(alpha * 255));
            SDL_RenderDrawLine(FlxG::renderer, 0, y, width, y);
        }
        
        SDL_SetRenderDrawColor(FlxG::renderer, r, g, b, 255);
        SDL_Rect solidRect = {0, height, width, height};
        SDL_RenderFillRect(FlxG::renderer, &solidRect);
    }
    
    SDL_SetRenderTarget(FlxG::renderer, nullptr);
    
    _back->loadGraphic(tex);
}

void FlxTransitionEffect::createHorizontalGradient(int width, int height, bool fromLeft) {
    int totalWidth = width * 2;
    
    SDL_Texture* tex = SDL_CreateTexture(FlxG::renderer, SDL_PIXELFORMAT_RGBA8888,
                                          SDL_TEXTUREACCESS_TARGET, totalWidth, height);
    if (!tex) return;
    
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(FlxG::renderer, tex);
    
    Uint8 r = static_cast<Uint8>(_data.color.red());
    Uint8 g = static_cast<Uint8>(_data.color.green());
    Uint8 b = static_cast<Uint8>(_data.color.blue());
    
    if (fromLeft) {
        SDL_SetRenderDrawColor(FlxG::renderer, r, g, b, 255);
        SDL_Rect solidRect = {0, 0, width, height};
        SDL_RenderFillRect(FlxG::renderer, &solidRect);
        
        for (int x = 0; x < width; x++) {
            float alpha = 1.0f - (static_cast<float>(x) / width);
            SDL_SetRenderDrawColor(FlxG::renderer, r, g, b, static_cast<Uint8>(alpha * 255));
            SDL_RenderDrawLine(FlxG::renderer, width + x, 0, width + x, height);
        }
    } else {
        for (int x = 0; x < width; x++) {
            float alpha = static_cast<float>(x) / width;
            SDL_SetRenderDrawColor(FlxG::renderer, r, g, b, static_cast<Uint8>(alpha * 255));
            SDL_RenderDrawLine(FlxG::renderer, x, 0, x, height);
        }
        
        SDL_SetRenderDrawColor(FlxG::renderer, r, g, b, 255);
        SDL_Rect solidRect = {width, 0, width, height};
        SDL_RenderFillRect(FlxG::renderer, &solidRect);
    }
    
    SDL_SetRenderTarget(FlxG::renderer, nullptr);
    
    _back->loadGraphic(tex);
}

FlxTransitionEffect::~FlxTransitionEffect() {
    if (_back) {
        delete _back;
        _back = nullptr;
    }
}

void FlxTransitionEffect::start(TransitionStatus newStatus) {
    _started = true;
    _tweenProgress = 0.0f;
    finished = false;
    
    if (newStatus == TransitionStatus::IN) {
        _endStatus = TransitionStatus::FULL;
    } else {
        _endStatus = TransitionStatus::EMPTY;
    }
    
    bool isIn = (newStatus == TransitionStatus::IN);
    setTweenValues(isIn, _data.direction.x, _data.direction.y);
    
    _back->x = _startX;
    _back->y = _startY;
    _back->alpha = _startAlpha;
}

void FlxTransitionEffect::setStatus(TransitionStatus newStatus) {
    switch (newStatus) {
        case TransitionStatus::FULL:
            _back->x = 0;
            _back->y = 0;
            _back->alpha = 1.0f;
            break;
        case TransitionStatus::EMPTY:
            if (_data.direction.x == 0 && _data.direction.y == 0) {
                _back->alpha = 0.0f;
            } else {
                _back->x = -_back->width * 2;
                _back->y = -_back->height * 2;
            }
            break;
        default:
            break;
    }
}

void FlxTransitionEffect::setTweenValues(bool isIn, float dirX, float dirY) {
    bool isOut = !isIn;
    float screenHeight = static_cast<float>(FlxG::height);
    float screenWidth = static_cast<float>(FlxG::width);
    float backWidth = _back->width;
    float backHeight = _back->height;
    
    _startX = 0;
    _startY = 0;
    _startAlpha = 1.0f;
    _targetX = 0;
    _targetY = 0;
    _targetAlpha = 1.0f;
    
    if (dirX == 0 && dirY == 0) {
        _startAlpha = isIn ? 0.0f : 1.0f;
        _targetAlpha = isOut ? 0.0f : 1.0f;
    } else if (dirX != 0 && dirY != 0) {
        if (dirX > 0) {
            _startX = isIn ? -backWidth : 0.0f;
            _targetX = isOut ? -backWidth : 0.0f;
        } else {
            _startX = isIn ? screenWidth : screenWidth - backWidth;
            _targetX = isOut ? screenWidth : screenWidth - backWidth;
        }
    } else if (dirX != 0) {
        if (dirX > 0) {
            _startX = isIn ? -backWidth : 0.0f;
            _targetX = isOut ? -backWidth : 0.0f;
        } else {
            _startX = isIn ? screenWidth : -backWidth / 2.0f;
            _targetX = isOut ? screenWidth : -backWidth / 2.0f;
        }
    } else {
        if (dirY > 0) {
            _startY = isIn ? -backHeight : 0.0f;
            _targetY = isOut ? -backHeight : 0.0f;
        } else {
            _startY = isIn ? screenHeight : -backHeight / 2.0f;
            _targetY = isOut ? screenHeight : -backHeight / 2.0f;
        }
    }
}

float FlxTransitionEffect::easeLinear(float t) {
    return t;
}

void FlxTransitionEffect::update(float elapsed) {
    if (finished) return;
    
    _tweenProgress += elapsed / _data.duration;
    
    if (_tweenProgress >= 1.0f) {
        _tweenProgress = 1.0f;
        finished = true;
    }
    
    float t = easeLinear(_tweenProgress);
    
    _back->x = _startX + (_targetX - _startX) * t;
    _back->y = _startY + (_targetY - _startY) * t;
    _back->alpha = _startAlpha + (_targetAlpha - _startAlpha) * t;
    
    if (_back) {
        _back->update(elapsed);
    }
    
    if (finished) {
        delayThenFinish();
    }
}

void FlxTransitionEffect::delayThenFinish() {
    if (finishCallback) {
        finishCallback();
    }
}

void FlxTransitionEffect::draw() {
    if (_back) {
        _back->draw();
    }
}

}
}
}