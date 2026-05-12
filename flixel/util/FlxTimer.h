#pragma once

#include <functional>
#include <vector>
#include "../FlxBasic.h"

namespace flixel {
namespace util {

class FlxTimerManager;

class FlxTimer {
public:
    FlxTimer(FlxTimerManager* manager = nullptr);
    ~FlxTimer();
    
    void destroy();
    
    FlxTimer* start(float Time = 1.0f, std::function<void(FlxTimer*)> OnComplete = nullptr, int Loops = 1);
    FlxTimer* reset(float NewTime = -1.0f);
    void cancel();
    void update(float elapsed);
    void onLoopFinished();
    
    float time = 0.0f;
    int loops = 0;
    bool active = false;
    bool finished = false;
    std::function<void(FlxTimer*)> onComplete;
    
    inline float getTimeLeft() const { return time - _timeCounter; }
    inline float getElapsedTime() const { return _timeCounter; }
    inline int getLoopsLeft() const { return loops - _loopsCounter; }
    inline int getElapsedLoops() const { return _loopsCounter; }
    inline float getProgress() const { return (time > 0) ? (_timeCounter / time) : 0.0f; }
    
    FlxTimerManager* manager = nullptr;
    
private:
    float _timeCounter = 0.0f;
    int _loopsCounter = 0;
    bool _inManager = false;
    
    friend class FlxTimerManager;
};

class FlxTimerManager : public FlxBasic {
public:
    FlxTimerManager();
    virtual ~FlxTimerManager();
    
    virtual void destroy() override;
    virtual void update(float elapsed) override;
    
    void add(FlxTimer* timer);
    void remove(FlxTimer* timer);
    void completeAll();
    void clear();
    void forEach(std::function<void(FlxTimer*)> function);
    
private:
    std::vector<FlxTimer*> _timers;
};

}
}