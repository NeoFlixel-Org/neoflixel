#include "FlxFlicker.h"
#include "../FlxG.h"
#include <iostream>

namespace flixel {
namespace effects {

std::map<FlxObject*, FlxFlicker*> FlxFlicker::_boundObjects;
std::vector<FlxFlicker*> FlxFlicker::_pool;

FlxFlicker* FlxFlicker::getPooled() {
    if (_pool.empty()) {
        return new FlxFlicker();
    }
    FlxFlicker* flicker = _pool.back();
    _pool.pop_back();
    return flicker;
}

void FlxFlicker::putPooled(FlxFlicker* flicker) {
    _pool.push_back(flicker);
}

FlxFlicker* FlxFlicker::flicker(FlxObject* Object, float Duration, float Interval, 
                                 bool EndVisibility, bool ForceRestart,
                                 std::function<void(FlxFlicker*)> CompletionCallback,
                                 std::function<void(FlxFlicker*)> ProgressCallback) {
    if (isFlickering(Object)) {
        if (ForceRestart) {
            stopFlickering(Object);
        } else {
            return _boundObjects[Object];
        }
    }
    
    if (Interval <= 0) {
        Interval = FlxG::elapsed;
    }
    
    FlxFlicker* flicker = getPooled();
    flicker->start(Object, Duration, Interval, EndVisibility, CompletionCallback, ProgressCallback);
    _boundObjects[Object] = flicker;
    return flicker;
}

bool FlxFlicker::isFlickering(FlxObject* Object) {
    return _boundObjects.find(Object) != _boundObjects.end();
}

void FlxFlicker::stopFlickering(FlxObject* Object) {
    auto it = _boundObjects.find(Object);
    if (it != _boundObjects.end()) {
        it->second->stop();
    }
}

void FlxFlicker::destroy() {
    object = nullptr;
    timer = nullptr;
    completionCallback = nullptr;
    progressCallback = nullptr;
}

void FlxFlicker::start(FlxObject* Object, float Duration, float Interval, bool EndVisibility,
                       std::function<void(FlxFlicker*)> CompletionCallback,
                       std::function<void(FlxFlicker*)> ProgressCallback) {
    object = Object;
    duration = Duration;
    interval = Interval;
    completionCallback = CompletionCallback;
    progressCallback = ProgressCallback;
    endVisibility = EndVisibility;
    
    int loops = static_cast<int>(duration / interval);
    timer = new util::FlxTimer();
    timer->start(interval, [this](util::FlxTimer* t) { flickerProgress(t); }, loops);
}

void FlxFlicker::stop() {
    if (timer) {
        timer->cancel();
        delete timer;
        timer = nullptr;
    }
    object->visible = true;
    release();
}

void FlxFlicker::release() {
    _boundObjects.erase(object);
    putPooled(this);
}

void FlxFlicker::flickerProgress(util::FlxTimer* Timer) {
    object->visible = !object->visible;
    
    if (progressCallback) {
        progressCallback(this);
    }
    
    if (Timer->loops > 0 && Timer->getLoopsLeft() == 0) {
        object->visible = endVisibility;
        if (completionCallback) {
            completionCallback(this);
        }
        delete timer;
        timer = nullptr;
        release();
    }
}
}
}