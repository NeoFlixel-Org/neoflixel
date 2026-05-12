#pragma once
#include <SDL2/SDL.h>

namespace flixel {
namespace input {

enum class FlxInputState {
    JUST_RELEASED = -1,
    RELEASED = 0,
    PRESSED = 1,
    JUST_PRESSED = 2
};

class FlxInput {
public:
    SDL_Scancode key;
    FlxInputState current = FlxInputState::RELEASED;
    FlxInputState last = FlxInputState::RELEASED;

    FlxInput() : key(SDL_SCANCODE_UNKNOWN) {}

    FlxInput(SDL_Scancode key) : key(key) {}

    void press() {
        last = current;
        current = (isPressed() ? FlxInputState::PRESSED : FlxInputState::JUST_PRESSED);
    }

    void release() {
        last = current;
        current = (isPressed() ? FlxInputState::JUST_RELEASED : FlxInputState::RELEASED);
    }

    void update() {
        if (last == FlxInputState::JUST_RELEASED && current == FlxInputState::JUST_RELEASED)
            current = FlxInputState::RELEASED;
        else if (last == FlxInputState::JUST_PRESSED && current == FlxInputState::JUST_PRESSED)
            current = FlxInputState::PRESSED;
        last = current;
    }

    void reset() {
        current = FlxInputState::RELEASED;
        last = FlxInputState::RELEASED;
    }

    bool justReleased() const { return current == FlxInputState::JUST_RELEASED; }
    bool released() const { return current == FlxInputState::RELEASED || justReleased(); }
    bool pressed() const { return current == FlxInputState::PRESSED || justPressed(); }
    bool justPressed() const { return current == FlxInputState::JUST_PRESSED; }

    void updateFromState(bool isDown) {
        last = current;
        if (isDown) {
            if (last == FlxInputState::RELEASED || last == FlxInputState::JUST_RELEASED)
                current = FlxInputState::JUST_PRESSED;
            else
                current = FlxInputState::PRESSED;
        } else {
            if (last == FlxInputState::PRESSED || last == FlxInputState::JUST_PRESSED)
                current = FlxInputState::JUST_RELEASED;
            else
                current = FlxInputState::RELEASED;
        }
    }

private:
    bool isPressed() const {
        const Uint8* state = SDL_GetKeyboardState(nullptr);
        return state[key];
    }
};

}
}
