#pragma once
#include <SDL.h>
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
        for (auto& pair : keys) {
            pair.second.update();
        }
    }

    void onEvent(const SDL_Event& event) {
        if (event.type == SDL_KEYDOWN) {
            auto it = keys.find(event.key.keysym.scancode);
            if (it != keys.end()) {
                it->second.press();
            }
        } else if (event.type == SDL_KEYUP) {
            auto it = keys.find(event.key.keysym.scancode);
            if (it != keys.end()) {
                it->second.release();
            }
        }
    }

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
