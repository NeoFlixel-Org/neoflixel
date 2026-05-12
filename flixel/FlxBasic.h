#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "../imports.h"

namespace flixel {

enum class FlixelType {
    BASIC,
    OBJECT,
    SPRITE,
    TEXT,
    TILEMAP
};

class FlxBasic {
public:
    FlxBasic();
    virtual ~FlxBasic();

    virtual void update(float elapsed);
    virtual void draw();
    virtual void destroy();
    virtual void kill();
    virtual void revive();

    bool active;
    bool exists;
    bool visible;
    FlixelType flixelType;

    float x = 0;
    float y = 0;

    float scaleX = 1.0f;
    float scaleY = 1.0f;

    float angle = 0.0f;

    float alpha = 1.0f;

    SDL_Color color = {255, 255, 255, 255};

protected:
    void initVars();
};

}