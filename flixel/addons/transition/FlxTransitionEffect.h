#pragma once

#include "TransitionData.h"
#include "../../FlxSprite.h"
#include "../../FlxG.h"
#include <functional>

namespace flixel {
namespace addons {
namespace transition {

enum class TransitionStatus {
    IN,
    OUT,
    EMPTY,
    FULL
};

class FlxTransitionEffect {
public:
    std::function<void()> finishCallback = nullptr;
    bool finished = false;
    
    FlxTransitionEffect(const TransitionData& data);
    ~FlxTransitionEffect();
    
    void start(TransitionStatus newStatus);
    void setStatus(TransitionStatus newStatus);
    void update(float elapsed);
    void draw();
    
    bool isFinished() const { return finished; }
    
private:
    TransitionData _data;
    FlxSprite* _back = nullptr;
    
    float _tweenProgress = 0.0f;
    float _targetX = 0.0f;
    float _targetY = 0.0f;
    float _targetAlpha = 1.0f;
    float _startX = 0.0f;
    float _startY = 0.0f;
    float _startAlpha = 1.0f;
    
    bool _started = false;
    TransitionStatus _endStatus = TransitionStatus::EMPTY;
    
    void setTweenValues(bool isIn, float dirX, float dirY);
    float easeLinear(float t);
    void delayThenFinish();
    void createVerticalGradient(int width, int height, bool fromTop);
    void createHorizontalGradient(int width, int height, bool fromLeft);
};

}
}
}