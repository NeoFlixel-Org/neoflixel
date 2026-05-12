#include "FlxSprite.h"
#include "FlxG.h"

namespace flixel {

FlxSprite::FlxSprite(float x, float y) 
    : FlxObject(x, y, 0, 0)
{
}

FlxSprite::~FlxSprite() {
    destroy();
}

void FlxSprite::loadGraphic(const std::string& path) {
    try {
        texture = FlxG::loadTexture(path);
    } catch (const std::exception&) {
        try {
            texture = FlxG::loadTexture("assets/images/logo/default.png");
            FlxG::log.warn("Sprite not found: " + path + ", loaded fallback image instead.");
        } catch (const std::exception& e) {
            FlxG::log.error("Failed to load both sprite and fallback image: " + std::string(e.what()));
            texture = nullptr;
            return;
        }
    }
    
    SDL_QueryTexture(texture, nullptr, nullptr, &sourceRect.w, &sourceRect.h);
    destRect.w = sourceRect.w;
    destRect.h = sourceRect.h;
    
    width = static_cast<float>(sourceRect.w);
    height = static_cast<float>(sourceRect.h);
    frameWidth = sourceRect.w;
    frameHeight = sourceRect.h;
    centerOrigin();
}

void FlxSprite::loadGraphic(SDL_Texture* newTexture) {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
    
    if (newTexture) {
        texture = newTexture;
    } else {
        try {
            texture = FlxG::loadTexture("assets/images/logo/default.png");
            FlxG::log.warn("Null texture provided, loaded fallback image instead.");
        } catch (const std::exception& e) {
            FlxG::log.error("Failed to load fallback image: " + std::string(e.what()));
            texture = nullptr;
            return;
        }
    }
    
    SDL_QueryTexture(texture, nullptr, nullptr, &sourceRect.w, &sourceRect.h);
    destRect.w = sourceRect.w;
    destRect.h = sourceRect.h;
    
    width = static_cast<float>(sourceRect.w);
    height = static_cast<float>(sourceRect.h);
    frameWidth = sourceRect.w;
    frameHeight = sourceRect.h;
    centerOrigin();
}

void FlxSprite::setScale(float x, float y) {
    scaleX = x;
    scaleY = y;
    updateHitbox();
}

void FlxSprite::setScale(float scale) {
    scaleX = scale;
    scaleY = scale;
    updateHitbox();
}

void FlxSprite::updateHitbox() {
    width = std::abs(scaleX) * static_cast<float>(sourceRect.w);
    height = std::abs(scaleY) * static_cast<float>(sourceRect.h);
    
    offsetX = -0.5f * (width - static_cast<float>(sourceRect.w));
    offsetY = -0.5f * (height - static_cast<float>(sourceRect.h));
    
    centerOrigin();
}

void FlxSprite::centerOffsets(bool adjustPosition) {
    offsetX = (static_cast<float>(sourceRect.w) - width) * 0.5f;
    offsetY = (static_cast<float>(sourceRect.h) - height) * 0.5f;
    
    if (adjustPosition) {
        x += offsetX;
        y += offsetY;
    }
}

void FlxSprite::centerOrigin() {
    originX = static_cast<float>(sourceRect.w) * 0.5f;
    originY = static_cast<float>(sourceRect.h) * 0.5f;
}

void FlxSprite::draw() {
    if (!texture || !visible) return;

    SDL_Rect srcRect = sourceRect;
    if (frames && animation) {
        int frameIdx = animation->getCurrentFrame();
        if (frameIdx >= 0 && frameIdx < frames->frames.size()) {
            srcRect = frames->frames[frameIdx].rect;
        }
    }

    destRect.x = static_cast<int>(x + offsetX);
    destRect.y = static_cast<int>(y + offsetY);
    destRect.w = static_cast<int>(srcRect.w * scaleX);
    destRect.h = static_cast<int>(srcRect.h * scaleY);

    Uint8 r = static_cast<Uint8>((color >> 16) & 0xFF);
    Uint8 g = static_cast<Uint8>((color >> 8) & 0xFF);
    Uint8 b = static_cast<Uint8>(color & 0xFF);
    SDL_SetTextureColorMod(texture, r, g, b);

    SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(alpha * 255));

    SDL_RenderCopyEx(
        FlxG::renderer,
        texture,
        &srcRect,
        &destRect,
        angle,
        nullptr,
        SDL_FLIP_NONE
    );
}

void FlxSprite::destroy() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    FlxObject::destroy();
}
}