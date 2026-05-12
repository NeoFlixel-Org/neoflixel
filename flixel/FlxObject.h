#pragma once

#include "FlxBasic.h"
#include "util/FlxAxes.h"
#include <SDL2/SDL.h>
#include <string>
#include "math/FlxPoint.h"
#include "FlxCamera.h"

namespace flixel {

enum class FlxDirectionFlags {
    NONE = 0,
    UP = 1 << 0,
    DOWN = 1 << 1,
    LEFT = 1 << 2,
    RIGHT = 1 << 3,
    ANY = UP | DOWN | LEFT | RIGHT
};

enum class CollisionDragType {
    NEVER = 0,
    ALWAYS = 1,
    IMMOVABLE = 2,
    HEAVIER = 3
};

class FlxCamera;

class FlxObject : public FlxBasic {
public:
    static constexpr float SEPARATE_BIAS = 4.0f;
    static bool defaultPixelPerfectPosition;
    static bool defaultMoves;

    FlxObject(float x = 0, float y = 0, float width = 0, float height = 0);
    virtual ~FlxObject();

    float x;
    float y;
    float width;
    float height;
    float angle;
    float alpha;
    bool visible;
    bool active;
    bool exists;
    bool moves;
    bool immovable;
    bool pixelPerfectPosition;
    bool pixelPerfectRender;
    float mass;
    float elasticity;
    Uint32 color;

    float velocityX;
    float velocityY;
    float accelerationX;
    float accelerationY;
    float dragX;
    float dragY;
    float maxVelocityX;
    float maxVelocityY;
    float angularVelocity;
    float angularAcceleration;
    float angularDrag;
    float maxAngular;

    FlxDirectionFlags touching;
    FlxDirectionFlags wasTouching;
    FlxDirectionFlags allowCollisions;
    CollisionDragType collisionXDrag;
    CollisionDragType collisionYDrag;

    FlxPoint scrollFactor;

    virtual void update(float elapsed);
    virtual void draw();
    virtual void destroy() override;
    virtual void kill();
    virtual void revive();
    virtual void reset(float x, float y);
    virtual void setPosition(float x, float y);
    virtual void setSize(float width, float height);
    virtual void setHitbox(float width, float height, float offsetX = 0, float offsetY = 0);
    virtual void updateHitbox();
    virtual void centerOrigin();
    virtual void screenCenter(util::FlxAxes axes = util::FlxAxes::XY);
    
    bool overlaps(FlxObject* object, bool inScreenSpace = false);
    bool overlapsAt(float x, float y, FlxObject* object, bool inScreenSpace = false);
    bool overlapsPoint(float x, float y, bool inScreenSpace = false);
    bool isOnScreen(FlxCamera* camera = nullptr);
    bool inWorldBounds();
    bool isTouching(FlxDirectionFlags direction);
    bool justTouched(FlxDirectionFlags direction);
    
    bool get_solid() const;
    void set_solid(bool value);

protected:
    void initVars();
    void initMotionVars();
    void updateMotion(float elapsed);
    float lastX;
    float lastY;

private:
    static bool allowCollisionDrag(CollisionDragType type, FlxObject* object1, FlxObject* object2);
};

inline FlxDirectionFlags operator|(FlxDirectionFlags a, FlxDirectionFlags b) {
    return static_cast<FlxDirectionFlags>(static_cast<int>(a) | static_cast<int>(b));
}

inline FlxDirectionFlags operator&(FlxDirectionFlags a, FlxDirectionFlags b) {
    return static_cast<FlxDirectionFlags>(static_cast<int>(a) & static_cast<int>(b));
}

inline bool operator>(FlxDirectionFlags a, FlxDirectionFlags b) {
    return static_cast<int>(a) > static_cast<int>(b);
}

inline bool hasAny(FlxDirectionFlags flags, FlxDirectionFlags check) {
    return (flags & check) != FlxDirectionFlags::NONE;
}
} 