#pragma once

#include "../FlxObject.h"
#include "../util/FlxTimer.h"
#include <functional>
#include <map>
#include <vector>

namespace flixel {
namespace effects {

class FlxFlicker {
public:
    static FlxFlicker* flicker(FlxObject* Object, float Duration = 1.0f, float Interval = 0.04f, 
                               bool EndVisibility = true, bool ForceRestart = true,
                               std::function<void(FlxFlicker*)> CompletionCallback = nullptr,
                               std::function<void(FlxFlicker*)> ProgressCallback = nullptr);
    
    static bool isFlickering(FlxObject* Object);
    static void stopFlickering(FlxObject* Object);
    
    void destroy();
    void stop();
    
    FlxObject* object = nullptr;
    bool endVisibility = false;
    util::FlxTimer* timer = nullptr;
    std::function<void(FlxFlicker*)> completionCallback;
    std::function<void(FlxFlicker*)> progressCallback;
    float duration = 0.0f;
    float interval = 0.0f;
    
private:
    FlxFlicker() = default;
    
    void start(FlxObject* Object, float Duration, float Interval, bool EndVisibility,
               std::function<void(FlxFlicker*)> CompletionCallback,
               std::function<void(FlxFlicker*)> ProgressCallback);
    
    void release();
    void flickerProgress(util::FlxTimer* Timer);
    
    static std::map<FlxObject*, FlxFlicker*> _boundObjects;
    static std::vector<FlxFlicker*> _pool;
    static FlxFlicker* getPooled();
    static void putPooled(FlxFlicker* flicker);
};

}
}