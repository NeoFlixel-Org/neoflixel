#pragma once
#include "FlxAnimation.h"
#include <map>

namespace flixel {
namespace animation {

class FlxAnimationController {
public:
    std::map<std::string, FlxAnimation> animations;
    std::string current;
    int currentFrame = 0;
    float timer = 0.0f;

    void addByPrefix(const std::string& name, const std::vector<int>& frames, int frameRate, bool looped);
    void play(const std::string& name);
    void update(float elapsed);
    int getCurrentFrame() const;
    
    std::string getName() const { return current; }
};
}
}