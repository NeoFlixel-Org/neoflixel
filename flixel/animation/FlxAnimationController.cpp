#include "FlxAnimationController.h"
#include <algorithm>
#include <cmath>

namespace flixel {
namespace animation {

void FlxAnimationController::addByPrefix(const std::string& name, const std::vector<int>& frames, int frameRate, bool looped) {
    animations[name] = {name, frames, frameRate, looped};
}

void FlxAnimationController::addByIndices(const std::string& name, const std::vector<int>& sourceFrames, const std::vector<int>& indices, int frameRate, bool looped) {
    std::vector<int> selectedFrames;
    for (int idx : indices) {
        if (idx >= 0 && idx < static_cast<int>(sourceFrames.size())) {
            selectedFrames.push_back(sourceFrames[idx]);
        }
    }
    animations[name] = {name, selectedFrames, frameRate, looped};
}

void FlxAnimationController::play(const std::string& name, bool force, int startFrame) {
    auto it = animations.find(name);
    if (it != animations.end()) {
        if (force || current != name) {
            current = name;
            currentFrame = startFrame;
            timer = 0.0f;
            finished = false;
            
            if (frameCallback && !it->second.frames.empty()) {
                frameCallback(current, currentFrame, it->second.frames[currentFrame]);
            }
        }
    }
}

void FlxAnimationController::update(float elapsed) {
    if (current.empty()) return;
    
    auto it = animations.find(current);
    if (it == animations.end()) return;
    
    const FlxAnimation& anim = it->second;
    if (anim.frames.empty()) return;
    
    timer += elapsed;
    const float frameTime = 1.0f / anim.frameRate;
    
    if (timer >= frameTime) {
        const int framesToAdvance = static_cast<int>(timer / frameTime);
        timer = std::fmod(timer, frameTime);
        
        int oldFrame = currentFrame;
        currentFrame += framesToAdvance;
        const int maxFrame = static_cast<int>(anim.frames.size());
        
        if (currentFrame >= maxFrame) {
            if (anim.looped) {
                currentFrame = currentFrame % maxFrame;
            } else {
                if (!finished) {
                    finished = true;
                    if (finishCallback) {
                        finishCallback(current);
                    }
                }
                if (finished) {
                    currentFrame = maxFrame - 1;
                }
            }
        }
        
        if (oldFrame != currentFrame && frameCallback) {
            frameCallback(current, currentFrame, anim.frames[currentFrame]);
        }
    }
}

}
}