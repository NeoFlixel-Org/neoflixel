#include "FlxBackdrop.h"
#include "../FlxG.h"
#include "../FlxCamera.h"
#include <cmath>

namespace flixel {
namespace addons {

FlxBackdrop::FlxBackdrop(const std::string& graphic, float scrollX, float scrollY,
                         bool repeatX_, bool repeatY_, int spaceX_, int spaceY_)
    : FlxSprite(), repeatX(repeatX_), repeatY(repeatY_), 
      spaceX(spaceX_), spaceY(spaceY_), useScaleHack(true),
      scrollW(0), scrollH(0), numTiles(0), ppointX(0.0f), ppointY(0.0f)
{
    scrollFactor.x = scrollX;
    scrollFactor.y = scrollY;
    
    if (!graphic.empty()) {
        loadGraphic(graphic);
    }
}

FlxBackdrop::FlxBackdrop(SDL_Texture* tex, float scrollX, float scrollY,
                         bool repeatX_, bool repeatY_, int spaceX_, int spaceY_)
    : FlxSprite(), repeatX(repeatX_), repeatY(repeatY_), 
      spaceX(spaceX_), spaceY(spaceY_), useScaleHack(true),
      scrollW(0), scrollH(0), numTiles(0), ppointX(0.0f), ppointY(0.0f)
{
    scrollFactor.x = scrollX;
    scrollFactor.y = scrollY;
    
    if (tex) {
        setTexture(tex);
    }
}

FlxBackdrop::~FlxBackdrop() {
    tileInfo.clear();
}

bool FlxBackdrop::loadGraphic(const std::string& path) {
    FlxSprite::loadGraphic(path);
    
    scrollW = static_cast<int>(frameWidth + spaceX);
    scrollH = static_cast<int>(frameHeight + spaceY);
    
    regenTileInfo();
    
    return true;
}

void FlxBackdrop::setTexture(SDL_Texture* tex) {
    if (!tex) return;
    
    texture = tex;
    
    int w, h;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    
    frameWidth = w;
    frameHeight = h;
    width = static_cast<float>(w);
    height = static_cast<float>(h);
    
    scrollW = w + spaceX;
    scrollH = h + spaceY;
    
    regenTileInfo();
}

void FlxBackdrop::regenTileInfo() {
    tileInfo.clear();
    numTiles = 0;
    
    float sx = std::abs(scale.x);
    float sy = std::abs(scale.y);
    
    int ssw = static_cast<int>(scrollW * sx);
    int ssh = static_cast<int>(scrollH * sy);
    
    int w = ssw;
    int h = ssh;
    
    if (repeatX) w += FlxG::width;
    if (repeatY) h += FlxG::height;
    
    width = static_cast<float>(w);
    height = static_cast<float>(h);
    frameWidth = w;
    frameHeight = h;
    
    float tileX = 0.0f;
    float tileY = 0.0f;
    
    while (tileY < h) {
        tileX = 0.0f;
        while (tileX < w) {
            tileInfo.push_back(tileX);
            tileInfo.push_back(tileY);
            numTiles++;
            tileX += ssw;
        }
        tileY += ssh;
    }
}

void FlxBackdrop::update(float elapsed) {
    FlxSprite::update(elapsed);
    
    x += velocity.x * elapsed;
    y += velocity.y * elapsed;
}

void FlxBackdrop::draw() {
    if (!visible || alpha <= 0.0f || !texture) {
        return;
    }
    
    FlxCamera* cam = camera ? camera : FlxG::camera;
    if (!cam) return;
    
    float ssw = scrollW * std::abs(scale.x);
    float ssh = scrollH * std::abs(scale.y);
    
    if (repeatX) {
        ppointX = std::fmod((x - offsetX - cam->scroll.x * scrollFactor.x), ssw);
        if (ppointX > 0.0f) ppointX -= ssw;
    } else {
        ppointX = (x - offsetX - cam->scroll.x * scrollFactor.x);
    }
    
    if (repeatY) {
        ppointY = std::fmod((y - offsetY - cam->scroll.y * scrollFactor.y), ssh);
        if (ppointY > 0.0f) ppointY -= ssh;
    } else {
        ppointY = (y - offsetY - cam->scroll.y * scrollFactor.y);
    }
    
    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = scrollW;
    srcRect.h = scrollH;
    
    if (frames && !frames->frames.empty()) {
        const auto& frame = frames->frames[0];
        srcRect.x = frame.rect.x;
        srcRect.y = frame.rect.y;
        srcRect.w = frame.rect.w;
        srcRect.h = frame.rect.h;
    }
    
    float scaleX = scale.x;
    float scaleY = scale.y;
    
    if (useScaleHack && cam) {
        scaleX += 1.0f / (frameWidth * cam->zoom);
        scaleY += 1.0f / (frameHeight * cam->zoom);
    }
    
    SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(alpha * 255.0f));
    SDL_SetTextureColorMod(texture, 
                          static_cast<Uint8>((color >> 16) & 0xFF),
                          static_cast<Uint8>((color >> 8) & 0xFF),
                          static_cast<Uint8>(color & 0xFF));
    
    for (int j = 0; j < numTiles; j++) {
        float currTileX = tileInfo[j * 2];
        float currTileY = tileInfo[(j * 2) + 1];
        
        float finalX = ppointX + currTileX;
        float finalY = ppointY + currTileY;
        
        if (cam && (scrollFactor.x != 0.0f || scrollFactor.y != 0.0f)) {
            finalX += cam->x;
            finalY += cam->y;
        }
        
        SDL_FRect destRect;
        destRect.x = finalX;
        destRect.y = finalY;
        destRect.w = srcRect.w * scaleX;
        destRect.h = srcRect.h * scaleY;
        
        if (flipX) {
            SDL_RenderCopyExF(FlxG::renderer, texture, &srcRect, &destRect, 
                             angle, nullptr, SDL_FLIP_HORIZONTAL);
        } else if (flipY) {
            SDL_RenderCopyExF(FlxG::renderer, texture, &srcRect, &destRect, 
                             angle, nullptr, SDL_FLIP_VERTICAL);
        } else {
            SDL_RenderCopyExF(FlxG::renderer, texture, &srcRect, &destRect, 
                             angle, nullptr, SDL_FLIP_NONE);
        }
    }
}
}
}