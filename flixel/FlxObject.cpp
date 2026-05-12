#include "FlxObject.h"
#include "FlxG.h"
#include <algorithm>

namespace flixel {

bool FlxObject::defaultPixelPerfectPosition = false;
bool FlxObject::defaultMoves = true;

FlxObject::FlxObject(float x, float y, float width, float height)
    : x(x)
    , y(y)
    , width(width)
    , height(height)
    , angle(0)
    , alpha(1.0f)
    , visible(true)
    , active(true)
    , exists(true)
    , moves(defaultMoves)
    , immovable(false)
    , pixelPerfectPosition(defaultPixelPerfectPosition)
    , pixelPerfectRender(false)
    , mass(1.0f)
    , elasticity(0.0f)
    , color(0xFFFFFFFF)
    , velocityX(0)
    , velocityY(0)
    , accelerationX(0)
    , accelerationY(0)
    , dragX(0)
    , dragY(0)
    , maxVelocityX(10000.0f)
    , maxVelocityY(10000.0f)
    , angularVelocity(0)
    , angularAcceleration(0)
    , angularDrag(0)
    , maxAngular(10000.0f)
    , touching(FlxDirectionFlags::NONE)
    , wasTouching(FlxDirectionFlags::NONE)
    , allowCollisions(FlxDirectionFlags::ANY)
    , collisionXDrag(CollisionDragType::IMMOVABLE)
    , collisionYDrag(CollisionDragType::NEVER)
    , lastX(x)
    , lastY(y)
{
    initVars();
}

FlxObject::~FlxObject() {
    destroy();
}

void FlxObject::initVars() {
    flixelType = FlixelType::OBJECT;
}

void FlxObject::initMotionVars() {
    velocityX = 0;
    velocityY = 0;
    accelerationX = 0;
    accelerationY = 0;
    dragX = 0;
    dragY = 0;
    maxVelocityX = 10000.0f;
    maxVelocityY = 10000.0f;
}

void FlxObject::update(float elapsed) {
    lastX = x;
    lastY = y;

    if (moves) {
        updateMotion(elapsed);
    }

    wasTouching = touching;
    touching = FlxDirectionFlags::NONE;
}

void FlxObject::updateMotion(float elapsed) {
    float velocityDelta = 0.5f * (std::min(maxAngular, angularVelocity + angularAcceleration * elapsed) - angularVelocity);
    angularVelocity += velocityDelta;
    angle += angularVelocity * elapsed;
    angularVelocity += velocityDelta;

    velocityDelta = 0.5f * (std::min(maxVelocityX, velocityX + accelerationX * elapsed) - velocityX);
    velocityX += velocityDelta;
    float delta = velocityX * elapsed;
    velocityX += velocityDelta;
    x += delta;

    velocityDelta = 0.5f * (std::min(maxVelocityY, velocityY + accelerationY * elapsed) - velocityY);
    velocityY += velocityDelta;
    delta = velocityY * elapsed;
    velocityY += velocityDelta;
    y += delta;
}

void FlxObject::draw() {}

void FlxObject::destroy() {
    FlxBasic::destroy();
}

void FlxObject::updateHitbox() {}

void FlxObject::setPosition(float x, float y) {
    this->x = x;
    this->y = y;
}

void FlxObject::reset(float x, float y) {
    touching = FlxDirectionFlags::NONE;
    wasTouching = FlxDirectionFlags::NONE;
    setPosition(x, y);
    lastX = x;
    lastY = y;
    velocityX = 0;
    velocityY = 0;
    revive();
}

bool FlxObject::overlaps(FlxObject* object, bool inScreenSpace) {
    if (!inScreenSpace) {
        return (object->x + object->width > x) && 
               (object->x < x + width) && 
               (object->y + object->height > y) && 
               (object->y < y + height);
    }

    float objectScreenX = object->x - (FlxG::camera->scroll.x * object->scrollFactor.x);
    float objectScreenY = object->y - (FlxG::camera->scroll.y * object->scrollFactor.y);
    float thisScreenX = x - (FlxG::camera->scroll.x * scrollFactor.x);
    float thisScreenY = y - (FlxG::camera->scroll.y * scrollFactor.y);

    return (objectScreenX + object->width > thisScreenX) && 
           (objectScreenX < thisScreenX + width) && 
           (objectScreenY + object->height > thisScreenY) && 
           (objectScreenY < thisScreenY + height);
}

bool FlxObject::overlapsAt(float x, float y, FlxObject* object, bool inScreenSpace) {
    if (!inScreenSpace) {
        return (object->x + object->width > x) && 
               (object->x < x + width) && 
               (object->y + object->height > y) && 
               (object->y < y + height);
    }

    float objectScreenX = object->x - (FlxG::camera->scroll.x * object->scrollFactor.x);
    float objectScreenY = object->y - (FlxG::camera->scroll.y * object->scrollFactor.y);
    float thisScreenX = x - (FlxG::camera->scroll.x * scrollFactor.x);
    float thisScreenY = y - (FlxG::camera->scroll.y * scrollFactor.y);

    return (objectScreenX + object->width > thisScreenX) && 
           (objectScreenX < thisScreenX + width) && 
           (objectScreenY + object->height > thisScreenY) && 
           (objectScreenY < thisScreenY + height);
}

bool FlxObject::overlapsPoint(float x, float y, bool inScreenSpace) {
    if (!inScreenSpace) {
        return (x >= this->x) && (x < this->x + width) && 
               (y >= this->y) && (y < this->y + height);
    }

    float screenX = x - (FlxG::camera->scroll.x * scrollFactor.x);
    float screenY = y - (FlxG::camera->scroll.y * scrollFactor.y);
    float thisScreenX = this->x - (FlxG::camera->scroll.x * scrollFactor.x);
    float thisScreenY = this->y - (FlxG::camera->scroll.y * scrollFactor.y);

    return (screenX >= thisScreenX) && (screenX < thisScreenX + width) && 
           (screenY >= thisScreenY) && (screenY < thisScreenY + height);
}

bool FlxObject::isOnScreen(FlxCamera* camera) {
    if (camera == nullptr) {
        camera = FlxG::camera;
    }

    float screenX = x - (camera->scroll.x * scrollFactor.x);
    float screenY = y - (camera->scroll.y * scrollFactor.y);

    return (screenX + width > camera->x) && 
           (screenX < camera->x + camera->width) && 
           (screenY + height > camera->y) && 
           (screenY < camera->y + camera->height);
}

bool FlxObject::inWorldBounds() {
    return FlxG::worldBounds.containsXY(x, y) && 
           FlxG::worldBounds.containsXY(x + width, y + height);
}

bool FlxObject::isTouching(FlxDirectionFlags direction) {
    return hasAny(touching, direction);
}

bool FlxObject::justTouched(FlxDirectionFlags direction) {
    return hasAny(touching, direction) && !hasAny(wasTouching, direction);
}

void FlxObject::setSize(float width, float height) {
    this->width = width;
    this->height = height;
}

bool FlxObject::get_solid() const {
    return (allowCollisions & FlxDirectionFlags::ANY) > FlxDirectionFlags::NONE;
}

void FlxObject::set_solid(bool value) {
    allowCollisions = value ? FlxDirectionFlags::ANY : FlxDirectionFlags::NONE;
}

bool FlxObject::allowCollisionDrag(CollisionDragType type, FlxObject* object1, FlxObject* object2) {
    if (!object2->active || !object2->moves) {
        return false;
    }

    switch (type) {
        case CollisionDragType::NEVER:
            return false;
        case CollisionDragType::ALWAYS:
            return true;
        case CollisionDragType::IMMOVABLE:
            return object2->immovable;
        case CollisionDragType::HEAVIER:
            return object2->immovable || object2->mass > object1->mass;
        default:
            return false;
    }
}

void FlxObject::kill() {}
void FlxObject::revive() {}
void FlxObject::setHitbox(float, float, float, float) {}
void FlxObject::centerOrigin() {}
void FlxObject::screenCenter(flixel::util::FlxAxes) {}
} 