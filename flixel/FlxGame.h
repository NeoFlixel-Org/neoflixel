#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <functional>

namespace flixel {

class FlxState;
class FlxG;

class FlxGame {
    friend class FlxG;

public:
    FlxGame(int gameWidth = 0, int gameHeight = 0, int updateFramerate = 60, int drawFramerate = 60, const std::string& title = "NeoFlixel Game");
    virtual ~FlxGame();

    void run();
    void update(float elapsed);
    void draw();

    void switchState(FlxState* newState);
    FlxState* getState() const { return currentState; }

    void onResize(int width, int height);
    void setFullscreen(bool fullscreen);
    void setTitle(const std::string& title);

    int getUpdateFramerate() const { return updateFramerate; }
    void setUpdateFramerate(int fps);
    int getDrawFramerate() const { return drawFramerate; }
    void setDrawFramerate(int fps);

protected:
    FlxState* currentState;
    FlxState* nextState;

    int updateFramerate;
    int drawFramerate;
    float stepMS;
    float stepSeconds;
    float accumulator;
    float maxAccumulation;

    int width;
    int height;
    bool running;
    bool fullscreen;
    std::string windowTitle;

    void handleEvents();
    void calculateElapsed();
};
}