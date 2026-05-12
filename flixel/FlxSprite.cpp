#include "FlxSprite.h"
#include "FlxG.h"

namespace flixel {

FlxSprite::FlxSprite(float x, float y) 
    : FlxObject(x, y, 0, 0), scale(1.0f, 1.0f), velocity(0.0f, 0.0f), acceleration(0.0f, 0.0f)
{
}

FlxSprite::~FlxSprite() {
    destroy();
}

void FlxSprite::loadGraphic(const std::string& path) {
    if (texture && ownsTexture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    
    try {
        texture = FlxG::loadTexture(path);
        ownsTexture = true;
    } catch (const std::exception&) {
        try {
            texture = FlxG::loadTexture(ASSETS_PATH "assets/images/logo/default.png");
            ownsTexture = true;
            FlxG::log.warn("Sprite not found: " + path + ", loaded fallback image instead.");
        } catch (const std::exception& e) {
            FlxG::log.error("Failed to load both sprite and fallback image: " + std::string(e.what()));
            texture = nullptr;
            ownsTexture = false;
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
    if (texture && ownsTexture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    
    if (newTexture) {
        texture = newTexture;
        ownsTexture = true;
    } else {
        try {
            texture = FlxG::loadTexture(ASSETS_PATH "assets/images/logo/default.png");
            ownsTexture = true;
            FlxG::log.warn("Null texture provided, loaded fallback image instead.");
        } catch (const std::exception& e) {
            FlxG::log.error("Failed to load fallback image: " + std::string(e.what()));
            texture = nullptr;
            ownsTexture = false;
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

void FlxSprite::makeGraphic(int w, int h, SDL_Color color) {
    if (ownsTexture && texture) {
        SDL_DestroyTexture(texture);
    }
    
    SDL_Renderer* renderer = FlxG::renderer;
    if (!renderer) return;
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!texture) return;
    
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, nullptr);
    
    ownsTexture = true;
    sourceRect = {0, 0, w, h};
    destRect = {0, 0, w, h};
    frameWidth = w;
    frameHeight = h;
    width = static_cast<float>(w);
    height = static_cast<float>(h);
    centerOrigin();
}

void FlxSprite::updateHitbox() {
    width = std::abs(scaleX) * static_cast<float>(frameWidth);
    height = std::abs(scaleY) * static_cast<float>(frameHeight);
    offsetX = -0.5f * (width - static_cast<float>(frameWidth));
    offsetY = -0.5f * (height - static_cast<float>(frameHeight));
    centerOrigin();
}

void FlxSprite::setGraphicSize(int Width, int Height) {
    if (Width <= 0 && Height <= 0)
        return;
    
    float newScaleX = static_cast<float>(Width) / frameWidth;
    float newScaleY = static_cast<float>(Height) / frameHeight;
    scale.set(newScaleX, newScaleY);
    
    if (Width <= 0)
        scale.x = newScaleY;
    else if (Height <= 0)
        scale.y = newScaleX;
    
    scaleX = scale.x;
    scaleY = scale.y;
}

void FlxSprite::centerOffsets(bool adjustPosition) {
    offsetX = (static_cast<float>(frameWidth) - width) * 0.5f;
    offsetY = (static_cast<float>(frameHeight) - height) * 0.5f;
    
    if (adjustPosition) {
        x += offsetX;
        y += offsetY;
    }
}

void FlxSprite::centerOrigin() {
    originX = static_cast<float>(frameWidth) * 0.5f;
    originY = static_cast<float>(frameHeight) * 0.5f;
}

void FlxSprite::update(float elapsed) {
    FlxObject::update(elapsed);
    
    velocity.x += acceleration.x * elapsed;
    velocity.y += acceleration.y * elapsed;
    x += velocity.x * elapsed;
    y += velocity.y * elapsed;
}

void FlxSprite::draw() {
    if (!texture || !visible || alpha <= 0.0f) return;
    const SDL_Rect* srcRect = &sourceRect;
    SDL_Rect frameRect;
    SDL_Rect clippedRect;
    float frameOffsetX = 0.0f;
    float frameOffsetY = 0.0f;
    
    if (frames && animation) {
        int frameIdx = animation->getCurrentFrame();
        if (frameIdx >= 0 && frameIdx < static_cast<int>(frames->frames.size())) {
            const auto& frame = frames->frames[frameIdx];
            frameRect = frame.rect;
            srcRect = &frameRect;
            frameOffsetX = static_cast<float>(frame.sourceSize.x) * scaleX;
            frameOffsetY = static_cast<float>(frame.sourceSize.y) * scaleY;
        }
    }
    
    if (useClipRect) {
        clippedRect = *srcRect;
        clippedRect.y += clipRect.y;
        clippedRect.h = clipRect.h;
        srcRect = &clippedRect;
        frameOffsetY += static_cast<float>(clipRect.y) * scaleY;
    }

    float camScrollX = 0.0f;
    float camScrollY = 0.0f;
    float camZoom = 1.0f;
    if (camera) {
        camScrollX = camera->scroll.x * scrollFactor.x;
        camScrollY = camera->scroll.y * scrollFactor.y;
        camZoom = camera->zoom;
    }

    float finalX = (x - offsetX + frameOffsetX - camScrollX) * camZoom;
    float finalY = (y - offsetY + frameOffsetY - camScrollY) * camZoom;
    float finalW = srcRect->w * scaleX * camZoom;
    float finalH = srcRect->h * scaleY * camZoom;

    SDL_FRect destRectF = {finalX, finalY, finalW, finalH};

    if (color != 0xFFFFFFFF) {
        const Uint8 r = static_cast<Uint8>((color >> 16) & 0xFF);
        const Uint8 g = static_cast<Uint8>((color >> 8) & 0xFF);
        const Uint8 b = static_cast<Uint8>(color & 0xFF);
        SDL_SetTextureColorMod(texture, r, g, b);
    } else {
        SDL_SetTextureColorMod(texture, 255, 255, 255);
    }

    SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(alpha * 255.0f));

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (flipX && flipY) {
        flip = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
    } else if (flipX) {
        flip = SDL_FLIP_HORIZONTAL;
    } else if (flipY) {
        flip = SDL_FLIP_VERTICAL;
    }

    if (angle == 0.0f && flip == SDL_FLIP_NONE) {
        SDL_RenderCopyF(FlxG::renderer, texture, srcRect, &destRectF);
    } else {
        SDL_RenderCopyExF(FlxG::renderer, texture, srcRect, &destRectF, angle, nullptr, flip);
    }
}

void FlxSprite::destroy() {
    if (texture && ownsTexture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
        ownsTexture = false;
    }
    FlxObject::destroy();
}
}