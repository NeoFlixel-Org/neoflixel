#pragma once
#include <SDL2/SDL.h>
#include <unordered_map>
#include <vector>
#include <set>
#include "FlxInput.h"

namespace flixel {
namespace input {

class FlxKeyboard {
public:
    std::unordered_map<SDL_Scancode, FlxInput> keys;

    FlxKeyboard() {
        for (int i = SDL_SCANCODE_UNKNOWN; i < SDL_NUM_SCANCODES; ++i) {
            keys[(SDL_Scancode)i] = FlxInput((SDL_Scancode)i);
        }
    }

    void update() {
        const Uint8* state = SDL_GetKeyboardState(nullptr);
        for (auto& pair : keys) {
            bool isDown = state[pair.first] != 0;
            pair.second.updateFromState(isDown);
        }
    }

    void onEvent(const SDL_Event& event) {}

    std::set<SDL_Scancode> pressed() const {
        std::set<SDL_Scancode> result;
        for (const auto& pair : keys)
            if (pair.second.pressed()) result.insert(pair.first);
        return result;
    }
    std::set<SDL_Scancode> justPressed() const {
        std::set<SDL_Scancode> result;
        for (const auto& pair : keys)
            if (pair.second.justPressed()) result.insert(pair.first);
        return result;
    }
    std::set<SDL_Scancode> justReleased() const {
        std::set<SDL_Scancode> result;
        for (const auto& pair : keys)
            if (pair.second.justReleased()) result.insert(pair.first);
        return result;
    }

    void reset() {
        for (auto& pair : keys) {
            pair.second.reset();
        }
    }
};

}
}
