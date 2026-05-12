#pragma once

#include "../../util/FlxColor.h"
#include "../../math/FlxPoint.h"
#include "../../math/FlxRect.h"

namespace flixel {
namespace addons {
namespace transition {

enum class TransitionType {
    NONE,
    TILES,
    FADE
};

struct TransitionData {
    TransitionType type = TransitionType::FADE;
    util::FlxColor color = util::FlxColor::BLACK;
    float duration = 1.0f;
    FlxPoint direction = FlxPoint(0, 0);
    math::FlxRect region;
    
    TransitionData() = default;
    
    TransitionData(TransitionType _type, util::FlxColor _color, float _duration, FlxPoint _direction = FlxPoint(0, 0))
        : type(_type), color(_color), duration(_duration), direction(_direction) {
        if (direction.x < -1.0f) direction.x = -1.0f;
        if (direction.x > 1.0f) direction.x = 1.0f;
        if (direction.y < -1.0f) direction.y = -1.0f;
        if (direction.y > 1.0f) direction.y = 1.0f;
    }
};

}
}
}