#pragma once

#include "FlxEase.h"
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace flixel {

class FlxObject;
class FlxSprite;

namespace tweens {

enum class FlxTweenType {
    ONESHOT,
    PERSIST,
    LOOPING,
    PINGPONG,
    BACKWARD
};

class FlxTween {
public:
    float duration;
    float time;
    bool active;
    bool finished;
    FlxTweenType type;
    EaseFunction ease;
    std::function<void()> onComplete;
    std::function<void()> onStart;
    std::function<void(FlxTween*)> onUpdate;
    float startDelay;
    float delayTimer;
    
    FlxTween(float duration = 1.0f);
    virtual ~FlxTween() = default;
    
    virtual void update(float elapsed);
    virtual void start();
    virtual void cancel();
    void destroy();
    
    float percent() const;
    float scale() const;
    
    virtual void* getObject() const { return nullptr; }
    
protected:
    bool _started;
    bool _backward;
};

class VarTween : public FlxTween {
public:
    void* object;
    std::map<std::string, float> startValues;
    std::map<std::string, float> targetValues;
    std::map<std::string, float*> propertyPointers;
    
    VarTween(float duration = 1.0f);
    
    void tween(void* obj, const std::map<std::string, float>& values, float dur);
    void addProperty(const std::string& name, float* ptr, float target);
    
    void update(float elapsed) override;
    void* getObject() const override { return object; }
};

class FlxTweenManager {
public:
    std::vector<FlxTween*> tweens;
    
    ~FlxTweenManager();
    
    void update(float elapsed);
    void clear();
    
    VarTween* tween(void* object, const std::map<std::string, float>& values, float duration = 1.0f, 
                    EaseFunction easeFunc = FlxEase::linear, 
                    std::function<void()> onComplete = nullptr,
                    std::function<void()> onStart = nullptr,
                    std::function<void(FlxTween*)> onUpdate = nullptr,
                    float startDelay = 0.0f,
                    FlxTweenType type = FlxTweenType::ONESHOT);
    
    void cancelTweensOf(void* object);
    void completeTweensOf(void* object);
};

extern FlxTweenManager* globalManager;

void init();
void cleanup();

VarTween* tween(void* object, const std::map<std::string, float>& values, float duration = 1.0f,
                EaseFunction easeFunc = FlxEase::linear,
                std::function<void()> onComplete = nullptr,
                std::function<void()> onStart = nullptr,
                std::function<void(FlxTween*)> onUpdate = nullptr,
                float startDelay = 0.0f,
                FlxTweenType type = FlxTweenType::ONESHOT);

void cancelTweensOf(void* object);
void completeTweensOf(void* object);

}
}