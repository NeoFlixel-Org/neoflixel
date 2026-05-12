#pragma once
#include <SDL.h>

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
    int pressCount = 0;

    FlxInput() : key(SDL_SCANCODE_UNKNOWN) {}

    FlxInput(SDL_Scancode key) : key(key) {}

    void press() {
        if (current == FlxInputState::PRESSED || current == FlxInputState::JUST_PRESSED) {
            pressCount++;
            return;
        }
        last = current;
        current = FlxInputState::JUST_PRESSED;
        pressCount = 0;
    }

    void release() {
        if (current == FlxInputState::RELEASED || current == FlxInputState::JUST_RELEASED) {
            return;
        }
        last = current;
        current = FlxInputState::JUST_RELEASED;
    }

    void update() {
        if (last == FlxInputState::JUST_RELEASED && current == FlxInputState::JUST_RELEASED)
            current = FlxInputState::RELEASED;
        else if (last == FlxInputState::JUST_PRESSED && current == FlxInputState::JUST_PRESSED) {
            if (pressCount > 0) {
                pressCount--;
                last = FlxInputState::RELEASED;
            } else {
                current = FlxInputState::PRESSED;
            }
        }
        last = current;
    }

    void reset() {
        current = FlxInputState::RELEASED;
        last = FlxInputState::RELEASED;
        pressCount = 0;
    }

    bool justReleased() const { return current == FlxInputState::JUST_RELEASED; }
    bool released() const { return current == FlxInputState::RELEASED || justReleased(); }
    bool pressed() const { return current == FlxInputState::PRESSED || justPressed(); }
    bool justPressed() const { return current == FlxInputState::JUST_PRESSED; }
};

}
}
