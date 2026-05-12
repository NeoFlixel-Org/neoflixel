#pragma once

#include "FlxObject.h"
#include <SDL2/SDL.h>
#include "flixel/graphics/frames/FlxAtlasFrames.h"
#include "flixel/animation/FlxAnimationController.h"

namespace flixel {

class FlxSprite : public FlxObject {
public:
    FlxSprite(float x = 0, float y = 0);
    virtual ~FlxSprite();

    void loadGraphic(const std::string& path);
    void loadGraphic(SDL_Texture* texture);

    void setScale(float x, float y);
    void setScale(float scale);
    float getScaleX() const { return scaleX; }
    float getScaleY() const { return scaleY; }

    virtual void updateHitbox() override;
    void centerOffsets(bool adjustPosition = false);
    void centerOrigin();

    virtual void draw() override;
    virtual void destroy() override;

    float width = 0;
    float height = 0;
    float offsetX = 0;
    float offsetY = 0;
    float originX = 0;
    float originY = 0;
    int frameWidth = 0;
    int frameHeight = 0;

    flixel::graphics::frames::FlxAtlasFrames* frames = nullptr;
    flixel::animation::FlxAnimationController* animation = nullptr;

protected:
    SDL_Texture* texture = nullptr;
    SDL_Rect sourceRect = {0, 0, 0, 0};
    SDL_Rect destRect = {0, 0, 0, 0};
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    Uint32 color = 0xFFFFFFFF;
};
}