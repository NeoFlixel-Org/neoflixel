#include "FlxBasic.h"

namespace flixel {

FlxBasic::FlxBasic() {
    initVars();
}

FlxBasic::~FlxBasic() {
    destroy();
}

void FlxBasic::initVars() {
    active = true;
    exists = true;
    visible = true;
    flixelType = FlixelType::BASIC;
}

void FlxBasic::update(float elapsed) {}

void FlxBasic::draw() {}

void FlxBasic::destroy() {}

void FlxBasic::kill() {
    active = false;
    exists = false;
}

void FlxBasic::revive() {
    active = true;
    exists = true;
}

} 