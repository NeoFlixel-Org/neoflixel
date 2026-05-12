#pragma once
#include <SDL.h>
#include <unordered_map>
#include <vector>

namespace flixel {
namespace input {

enum class FlxButtonState {
    IDLE,
    PRESSED,
    JUST_PRESSED,
    JUST_RELEASED
};

class FlxGamepad {
public:
    std::unordered_map<SDL_GameControllerButton, FlxButtonState> buttons;
    std::unordered_map<SDL_GameControllerButton, FlxButtonState> previousButtons;
    SDL_GameController* controller;
    
    FlxGamepad();
    ~FlxGamepad();
    
    void init();
    void close();
    void update();
    void onEvent(const SDL_Event& event);
    
    bool pressed(SDL_GameControllerButton button) const;
    bool justPressed(SDL_GameControllerButton button) const;
    bool justReleased(SDL_GameControllerButton button) const;
    
    Sint16 getAxis(SDL_GameControllerAxis axis) const;
    bool isConnected() const { return controller != nullptr; }
    
    void reset();

private:
    std::unordered_map<SDL_GameControllerAxis, Sint16> axisState;
};
}
}