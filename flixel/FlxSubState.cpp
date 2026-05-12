#include "FlxSubState.h"
#include "FlxG.h"
#include "FlxCamera.h"

namespace flixel {

FlxSubState::FlxSubState(const util::FlxColor& bgColor)
    : FlxState()
    , openCallback(nullptr)
    , closeCallback(nullptr)
    , _parentState(nullptr)
    , _bgColor(bgColor)
    , _created(false)
{
}

FlxSubState::~FlxSubState() {
    destroy();
}

void FlxSubState::draw() {
    for (auto* camera : cameras) {
        if (camera) {
            camera->fill(_bgColor);
        }
    }

    FlxState::draw();
}

void FlxSubState::destroy() {
    openCallback = nullptr;
    closeCallback = nullptr;
    _parentState = nullptr;
    FlxState::destroy();
}

void FlxSubState::close() {
    if (_parentState && _parentState->subState == this) {
        _parentState->closeSubState();
    }
}

void FlxSubState::setBgColor(const util::FlxColor& value) {
    _bgColor = value;
}
}