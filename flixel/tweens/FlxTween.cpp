#include "FlxTween.h"
#include "../FlxObject.h"
#include "../FlxSprite.h"
#include <algorithm>
#include <cmath>

namespace flixel {
    namespace tweens {

        FlxTweenManager* globalManager = nullptr;

        void init() {
            if (!globalManager) {
                globalManager = new FlxTweenManager();
            }
        }

        void cleanup() {
            if (globalManager) {
                delete globalManager;
                globalManager = nullptr;
            }
        }

        FlxTween::FlxTween(float dur)
            : duration(dur), time(0.0f), active(false), finished(false),
            type(FlxTweenType::ONESHOT), ease(nullptr),
            startDelay(0.0f), delayTimer(0.0f), _started(false), _backward(false)
        {
            ease = FlxEase::linear;
        }

        void FlxTween::update(float elapsed) {
            if (!active || finished) return;
            
            if (!_started) {
                if (delayTimer < startDelay) {
                    delayTimer += elapsed;
                    return;
                }
                _started = true;
                if (onStart) onStart();
            }
            
            time += elapsed;
            if (time >= duration) {
                time = duration;
                finished = true;
            }
            
            if (onUpdate) onUpdate(this);
            
            if (finished) {
                if (onComplete) onComplete();
                
                switch (type) {
                    case FlxTweenType::LOOPING:
                        time = 0.0f;
                        finished = false;
                        break;
                    case FlxTweenType::PINGPONG:
                        time = 0.0f;
                        finished = false;
                        _backward = !_backward;
                        break;
                    case FlxTweenType::ONESHOT:
                        active = false;
                        break;
                    default:
                        active = false;
                        break;
                }
            }
        }

        void FlxTween::start() {
            active = true;
            finished = false;
            time = 0.0f;
            _started = false;
            delayTimer = 0.0f;
        }

        void FlxTween::cancel() {
            active = false;
            finished = true;
        }

        void FlxTween::destroy() {
            onComplete = nullptr;
            onStart = nullptr;
            onUpdate = nullptr;
        }

        float FlxTween::percent() const {
            return duration > 0.0f ? time / duration : 0.0f;
        }

        float FlxTween::scale() const {
            float t = percent();
            if (_backward) t = 1.0f - t;
            return ease ? ease(t) : t;
        }

        VarTween::VarTween(float duration)
            : FlxTween(duration), object(nullptr)
        {
        }

        void VarTween::tween(void* obj, const std::map<std::string, float>& values, float dur) {
            object = obj;
            duration = dur;
            targetValues = values;
            startValues.clear();
            propertyPointers.clear();
        }

        void VarTween::addProperty(const std::string& name, float* ptr, float target) {
            if (ptr) {
                propertyPointers[name] = ptr;
                startValues[name] = *ptr;
                targetValues[name] = target;
            }
        }

        void VarTween::update(float elapsed) {
            if (!active || !object) return;
            
            FlxTween::update(elapsed);
            
            float t = scale();
            
            for (auto& prop : propertyPointers) {
                const std::string& name = prop.first;
                float* ptr = prop.second;
                
                if (ptr && startValues.count(name) && targetValues.count(name)) {
                    float start = startValues[name];
                    float target = targetValues[name];
                    *ptr = start + (target - start) * t;
                }
            }
        }

        FlxTweenManager::~FlxTweenManager() {
            clear();
        }

        void FlxTweenManager::update(float elapsed) {
            for (auto it = tweens.begin(); it != tweens.end();) {
                FlxTween* tween = *it;
                tween->update(elapsed);
                
                if (tween->type == FlxTweenType::ONESHOT && tween->finished) {
                    delete tween;
                    it = tweens.erase(it);
                } else {
                    ++it;
                }
            }
        }

        void FlxTweenManager::clear() {
            for (auto tween : tweens) {
                delete tween;
            }
            tweens.clear();
        }

        VarTween* FlxTweenManager::tween(void* object, const std::map<std::string, float>& values, float duration,
                                        EaseFunction easeFunc, std::function<void()> onComplete,
                                        std::function<void()> onStart, std::function<void(FlxTween*)> onUpdate,
                                        float startDelay, FlxTweenType type)
        {
            VarTween* varTween = new VarTween(duration);
            varTween->object = object;
            varTween->ease = easeFunc ? easeFunc : FlxEase::linear;
            varTween->onComplete = onComplete;
            varTween->onStart = onStart;
            varTween->onUpdate = onUpdate;
            varTween->startDelay = startDelay;
            varTween->type = type;
            varTween->targetValues = values;
            
            FlxSprite* sprite = static_cast<FlxSprite*>(object);
            for (const auto& pair : values) {
                const std::string& propName = pair.first;
                float targetValue = pair.second;
                
                if (propName == "alpha") {
                    varTween->addProperty("alpha", &sprite->alpha, targetValue);
                }
                else if (propName == "x") {
                    varTween->addProperty("x", &sprite->x, targetValue);
                }
                else if (propName == "y") {
                    varTween->addProperty("y", &sprite->y, targetValue);
                }
                else if (propName == "angle") {
                    varTween->addProperty("angle", &sprite->angle, targetValue);
                }
                else if (propName == "scaleX") {
                    varTween->addProperty("scaleX", &sprite->scale.x, targetValue);
                }
                else if (propName == "scaleY") {
                    varTween->addProperty("scaleY", &sprite->scale.y, targetValue);
                }
            }
            
            varTween->start();
            tweens.push_back(varTween);
            
            return varTween;
        }

        void FlxTweenManager::cancelTweensOf(void* object) {
            for (auto it = tweens.begin(); it != tweens.end();) {
                if ((*it)->getObject() == object) {
                    delete *it;
                    it = tweens.erase(it);
                } else {
                    ++it;
                }
            }
        }

        void FlxTweenManager::completeTweensOf(void* object) {
            for (auto tween : tweens) {
                if (tween->getObject() == object) {
                    VarTween* varTween = static_cast<VarTween*>(tween);
                    varTween->time = varTween->duration;
                    varTween->update(0.0f);
                    varTween->cancel();
                }
            }
        }

        VarTween* tween(void* object, const std::map<std::string, float>& values, float duration,
                        EaseFunction easeFunc, std::function<void()> onComplete,
                        std::function<void()> onStart, std::function<void(FlxTween*)> onUpdate,
                        float startDelay, FlxTweenType type)
        {
            if (!globalManager) init();
            return globalManager->tween(object, values, duration, easeFunc, onComplete, onStart, onUpdate, startDelay, type);
        }

        void cancelTweensOf(void* object) {
            if (globalManager) globalManager->cancelTweensOf(object);
        }

        void completeTweensOf(void* object) {
            if (globalManager) globalManager->completeTweensOf(object);
        }
    }
}