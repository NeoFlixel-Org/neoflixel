#include "FlxGamepad.h"
#include <iostream>
#include "../../imports.h" // me when

namespace flixel {
namespace input {

FlxGamepad::FlxGamepad() : controller(nullptr) {
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i) {
        SDL_GameControllerButton btn = static_cast<SDL_GameControllerButton>(i);
        buttons[btn] = FlxButtonState::IDLE;
        previousButtons[btn] = FlxButtonState::IDLE;
    }
    
    for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; ++i) {
        axisState[static_cast<SDL_GameControllerAxis>(i)] = 0;
    }
}

FlxGamepad::~FlxGamepad() {
    close();
}

void FlxGamepad::init() {
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "Failed to initialize game controller subsystem: " << SDL_GetError() << std::endl;
        return;
    }

    std::cout << "Loading game controller mappings from assets/data/gamecontrollerdb.txt" << std::endl;
    int mappingsLoaded = SDL_GameControllerAddMappingsFromFile(ASSETS_PATH "assets/data/gamecontrollerdb.txt");
    if (mappingsLoaded > 0) {
        std::cout << "Loaded " << mappingsLoaded << " controller mappings" << std::endl;
    }

    int numJoysticks = SDL_NumJoysticks();
    std::cout << "Found " << numJoysticks << " joystick devices" << std::endl;

    for (int i = 0; i < numJoysticks; i++) {
        if (SDL_IsGameController(i)) {
            std::cout << "Joystick " << i << " is a game controller: " << SDL_GameControllerNameForIndex(i) << std::endl;
            controller = SDL_GameControllerOpen(i);
            if (controller) {
                std::cout << "Successfully opened controller: " << SDL_GameControllerName(controller) << std::endl;
                break;
            } else {
                std::cerr << "Failed to open controller: " << SDL_GetError() << std::endl;
            }
        } else {
            std::cout << "Joystick " << i << " is not a game controller" << std::endl;
        }
    }
    
    if (!controller) {
        std::cout << "No game controller found" << std::endl;
    }
}

void FlxGamepad::close() {
    if (controller) {
        SDL_GameControllerClose(controller);
        controller = nullptr;
    }
}

void FlxGamepad::update() {
    previousButtons = buttons;
    
    if (controller) {
        for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
            SDL_GameControllerButton button = static_cast<SDL_GameControllerButton>(i);
            bool isPressed = SDL_GameControllerGetButton(controller, button) == 1;
            
            FlxButtonState prevState = previousButtons[button];
            
            if (isPressed) {
                if (prevState == FlxButtonState::IDLE || prevState == FlxButtonState::JUST_RELEASED) {
                    buttons[button] = FlxButtonState::JUST_PRESSED;
                } else {
                    buttons[button] = FlxButtonState::PRESSED;
                }
            } else {
                if (prevState == FlxButtonState::PRESSED || prevState == FlxButtonState::JUST_PRESSED) {
                    buttons[button] = FlxButtonState::JUST_RELEASED;
                } else {
                    buttons[button] = FlxButtonState::IDLE;
                }
            }
        }
        
        for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; i++) {
            SDL_GameControllerAxis axis = static_cast<SDL_GameControllerAxis>(i);
            axisState[axis] = SDL_GameControllerGetAxis(controller, axis);
        }
    }
}

void FlxGamepad::onEvent(const SDL_Event& event) {
    if (event.type == SDL_CONTROLLERDEVICEADDED) {
        if (!controller) {
            controller = SDL_GameControllerOpen(event.cdevice.which);
            if (controller) {
                std::cout << "Controller connected: " << SDL_GameControllerName(controller) << std::endl;
            }
        }
    } else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
        if (controller && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller)) == event.cdevice.which) {
            std::cout << "Controller disconnected" << std::endl;
            SDL_GameControllerClose(controller);
            controller = nullptr;
        }
    }
}

bool FlxGamepad::pressed(SDL_GameControllerButton button) const {
    auto it = buttons.find(button);
    if (it == buttons.end()) return false;
    return it->second == FlxButtonState::PRESSED || it->second == FlxButtonState::JUST_PRESSED;
}

bool FlxGamepad::justPressed(SDL_GameControllerButton button) const {
    auto it = buttons.find(button);
    if (it == buttons.end()) return false;
    return it->second == FlxButtonState::JUST_PRESSED;
}

bool FlxGamepad::justReleased(SDL_GameControllerButton button) const {
    auto it = buttons.find(button);
    if (it == buttons.end()) return false;
    return it->second == FlxButtonState::JUST_RELEASED;
}

Sint16 FlxGamepad::getAxis(SDL_GameControllerAxis axis) const {
    auto it = axisState.find(axis);
    return it != axisState.end() ? it->second : 0;
}

void FlxGamepad::reset() {
    for (auto& pair : buttons) {
        pair.second = FlxButtonState::IDLE;
    }
    for (auto& pair : previousButtons) {
        pair.second = FlxButtonState::IDLE;
    }
}
}
}
