#pragma once

#include "FlxTween.h"
#include "../FlxSprite.h"

namespace flixel {
    namespace tweens {

        inline VarTween* tweenX(FlxSprite* sprite, float targetX, float duration, 
                                EaseFunction ease = FlxEase::linear,
                                std::function<void()> onComplete = nullptr,
                                float startDelay = 0.0f) {
            if (!globalManager || !sprite) return nullptr;
            
            VarTween* tween = new VarTween(duration);
            tween->object = sprite;
            tween->ease = ease;
            tween->onComplete = onComplete;
            tween->type = FlxTweenType::ONESHOT;
            tween->startDelay = startDelay;
            tween->addProperty("x", &sprite->x, targetX);
            tween->start();
            
            globalManager->tweens.push_back(tween);
            return tween;
        }

        inline VarTween* tweenY(FlxSprite* sprite, float targetY, float duration,
                                EaseFunction ease = FlxEase::linear,
                                std::function<void()> onComplete = nullptr,
                                float startDelay = 0.0f) {
            if (!globalManager || !sprite) return nullptr;
            
            VarTween* tween = new VarTween(duration);
            tween->object = sprite;
            tween->ease = ease;
            tween->onComplete = onComplete;
            tween->type = FlxTweenType::ONESHOT;
            tween->startDelay = startDelay;
            tween->addProperty("y", &sprite->y, targetY);
            tween->start();
            
            globalManager->tweens.push_back(tween);
            return tween;
        }

        inline VarTween* tweenXY(FlxSprite* sprite, float targetX, float targetY, float duration,
                                EaseFunction ease = FlxEase::linear,
                                std::function<void()> onComplete = nullptr,
                                float startDelay = 0.0f) {
            if (!globalManager || !sprite) return nullptr;
            
            VarTween* tween = new VarTween(duration);
            tween->object = sprite;
            tween->ease = ease;
            tween->onComplete = onComplete;
            tween->type = FlxTweenType::ONESHOT;
            tween->startDelay = startDelay;
            tween->addProperty("x", &sprite->x, targetX);
            tween->addProperty("y", &sprite->y, targetY);
            tween->start();
            
            globalManager->tweens.push_back(tween);
            return tween;
        }

        inline VarTween* tweenAlpha(FlxSprite* sprite, float targetAlpha, float duration,
                                    EaseFunction ease = FlxEase::linear,
                                    std::function<void()> onComplete = nullptr,
                                    float startDelay = 0.0f) {
            if (!globalManager || !sprite) return nullptr;
            
            VarTween* tween = new VarTween(duration);
            tween->object = sprite;
            tween->ease = ease;
            tween->onComplete = onComplete;
            tween->type = FlxTweenType::ONESHOT;
            tween->startDelay = startDelay;
            tween->addProperty("alpha", &sprite->alpha, targetAlpha);
            tween->start();
            
            globalManager->tweens.push_back(tween);
            return tween;
        }

        inline VarTween* tweenScaleX(FlxSprite* sprite, float targetScale, float duration,
                                    EaseFunction ease = FlxEase::linear,
                                    std::function<void()> onComplete = nullptr,
                                    float startDelay = 0.0f) {
            if (!globalManager || !sprite) return nullptr;
            
            VarTween* tween = new VarTween(duration);
            tween->object = sprite;
            tween->ease = ease;
            tween->onComplete = onComplete;
            tween->type = FlxTweenType::ONESHOT;
            tween->startDelay = startDelay;
            tween->addProperty("scale.x", &sprite->scale.x, targetScale);
            tween->start();
            
            globalManager->tweens.push_back(tween);
            return tween;
        }

        inline VarTween* tweenScaleY(FlxSprite* sprite, float targetScale, float duration,
                                    EaseFunction ease = FlxEase::linear,
                                    std::function<void()> onComplete = nullptr,
                                    float startDelay = 0.0f) {
            if (!globalManager || !sprite) return nullptr;
            
            VarTween* tween = new VarTween(duration);
            tween->object = sprite;
            tween->ease = ease;
            tween->onComplete = onComplete;
            tween->type = FlxTweenType::ONESHOT;
            tween->startDelay = startDelay;
            tween->addProperty("scale.y", &sprite->scale.y, targetScale);
            tween->start();
            
            globalManager->tweens.push_back(tween);
            return tween;
        }

        inline VarTween* tweenAngle(FlxSprite* sprite, float targetAngle, float duration,
                                    EaseFunction ease = FlxEase::linear,
                                    std::function<void()> onComplete = nullptr,
                                    float startDelay = 0.0f) {
            if (!globalManager || !sprite) return nullptr;
            
            VarTween* tween = new VarTween(duration);
            tween->object = sprite;
            tween->ease = ease;
            tween->onComplete = onComplete;
            tween->type = FlxTweenType::ONESHOT;
            tween->startDelay = startDelay;
            tween->addProperty("angle", &sprite->angle, targetAngle);
            tween->start();
            
            globalManager->tweens.push_back(tween);
            return tween;
        }
    }
}