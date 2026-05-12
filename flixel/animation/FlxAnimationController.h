#pragma once
#include "FlxAnimation.h"
#include <unordered_map>
#include <string>
#include <functional>

namespace flixel {
namespace animation {

class FlxAnimationController {
public:
    std::unordered_map<std::string, FlxAnimation> animations;
    std::string current;
    int currentFrame = 0;
    float timer = 0.0f;
    
    std::function<void(const std::string&)> finishCallback;
    std::function<void(const std::string&, int, int)> frameCallback;

    void addByPrefix(const std::string& name, const std::vector<int>& frames, int frameRate, bool looped);
    void addByIndices(const std::string& name, const std::vector<int>& sourceFrames, const std::vector<int>& indices, int frameRate, bool looped);
    void play(const std::string& name, bool force = false, int startFrame = 0);
    void update(float elapsed);
    
    inline int getCurrentFrame() const {
        if (current.empty()) return 0;
        auto it = animations.find(current);
        if (it == animations.end() || currentFrame >= it->second.frames.size()) return 0;
        return it->second.frames[currentFrame];
    }
    
    inline bool isFinished() const {
        if (current.empty()) return true;
        auto it = animations.find(current);
        if (it == animations.end()) return true;
        const FlxAnimation& anim = it->second;
        if (anim.looped) return false;
        return currentFrame >= static_cast<int>(anim.frames.size()) - 1;
    }
    
    inline const std::string& getName() const { return current; }    
    bool finished = false;
};
}
}