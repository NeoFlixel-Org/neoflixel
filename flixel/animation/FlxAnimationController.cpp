#include "FlxAnimationController.h"

namespace flixel {
namespace animation {

void FlxAnimationController::addByPrefix(const std::string& name, const std::vector<int>& frames, int frameRate, bool looped) {
    animations[name] = {name, frames, frameRate, looped};
}

void FlxAnimationController::play(const std::string& name) {
    if (animations.count(name)) {
        current = name;
        currentFrame = 0;
        timer = 0.0f;
    }
}

void FlxAnimationController::update(float elapsed) {
    if (current.empty() || !animations.count(current)) return;
    auto& anim = animations[current];
    timer += elapsed;
    float frameTime = 1.0f / anim.frameRate;
    while (timer >= frameTime) {
        timer -= frameTime;
        currentFrame++;
        if (currentFrame >= anim.frames.size()) {
            if (anim.looped) currentFrame = 0;
            else currentFrame = anim.frames.size() - 1;
        }
    }
}

int FlxAnimationController::getCurrentFrame() const {
    if (current.empty() || !animations.count(current)) return 0;
    return animations.at(current).frames[currentFrame];
}

}
}