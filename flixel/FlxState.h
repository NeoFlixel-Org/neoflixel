#pragma once

#include "FlxBasic.h"
#include "FlxCamera.h"
#include <vector>

namespace flixel {

class FlxSubState;

class FlxState {
public:
    FlxState() = default;
    virtual ~FlxState() = default;

    virtual void create() = 0;
    virtual void update(float elapsed);
    virtual void draw();
    virtual void destroy();

    void add(FlxBasic* object);
    void remove(FlxBasic* object, bool destroy = true);
    void clear(bool destroy = true);

    void openSubState(FlxSubState* substate);
    void closeSubState();
    FlxSubState* subState = nullptr;

protected:
    std::vector<FlxBasic*> members;
    std::vector<FlxCamera*> cameras;
};
}