#include "FlxTimer.h"
#include "../FlxG.h"
#include <algorithm>
#include <cmath>

namespace flixel {
namespace util {

FlxTimer::FlxTimer(FlxTimerManager* mgr) 
    : manager(mgr ? mgr : FlxG::timers)
{
}

FlxTimer::~FlxTimer() {
    destroy();
}

void FlxTimer::destroy() {
    onComplete = nullptr;
}

FlxTimer* FlxTimer::start(float Time, std::function<void(FlxTimer*)> OnComplete, int Loops) {
    if (manager != nullptr && !_inManager) {
        manager->add(this);
        _inManager = true;
    }
    
    active = true;
    finished = false;
    time = std::abs(Time);
    
    if (Loops < 0)
        Loops *= -1;
    
    loops = Loops;
    onComplete = OnComplete;
    _timeCounter = 0.0f;
    _loopsCounter = 0;
    
    return this;
}

FlxTimer* FlxTimer::reset(float NewTime) {
    if (NewTime < 0)
        NewTime = time;
    
    start(NewTime, onComplete, loops);
    return this;
}

void FlxTimer::cancel() {
    finished = true;
    active = false;
    
    if (manager != nullptr && _inManager) {
        manager->remove(this);
        _inManager = false;
    }
}

void FlxTimer::update(float elapsed) {
    _timeCounter += elapsed;
    
    while ((_timeCounter >= time) && active && !finished) {
        _timeCounter -= time;
        _loopsCounter++;
        
        if (loops > 0 && (_loopsCounter >= loops)) {
            finished = true;
        }
    }
}

void FlxTimer::onLoopFinished() {
    if (finished)
        cancel();
    
    if (onComplete != nullptr)
        onComplete(this);
}

FlxTimerManager::FlxTimerManager() {
    visible = false;
}

FlxTimerManager::~FlxTimerManager() {
    destroy();
}

void FlxTimerManager::destroy() {
    clear();
    FlxBasic::destroy();
}

void FlxTimerManager::update(float elapsed) {
    std::vector<FlxTimer*> loopedTimers;
    
    for (auto timer : _timers) {
        if (timer->active && !timer->finished && timer->time >= 0) {
            int timerLoops = timer->getElapsedLoops();
            timer->update(elapsed);
            
            if (timerLoops != timer->getElapsedLoops()) {
                loopedTimers.push_back(timer);
            }
        }
    }
    
    for (auto timer : loopedTimers) {
        timer->onLoopFinished();
    }
}

void FlxTimerManager::add(FlxTimer* timer) {
    _timers.push_back(timer);
}

void FlxTimerManager::remove(FlxTimer* timer) {
    auto it = std::find(_timers.begin(), _timers.end(), timer);
    if (it != _timers.end()) {
        _timers.erase(it);
    }
}

void FlxTimerManager::completeAll() {
    std::vector<FlxTimer*> timersToFinish;
    for (auto timer : _timers) {
        if (timer->loops > 0 && timer->active) {
            timersToFinish.push_back(timer);
        }
    }
    
    for (auto timer : timersToFinish) {
        while (!timer->finished) {
            timer->update(timer->getTimeLeft());
            timer->onLoopFinished();
        }
    }
}

void FlxTimerManager::clear() {
    _timers.clear();
}

void FlxTimerManager::forEach(std::function<void(FlxTimer*)> function) {
    for (auto timer : _timers) {
        function(timer);
    }
}

}
}