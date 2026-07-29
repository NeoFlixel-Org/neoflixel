#include "FlxGame.h"
#include "FlxG.h"
#include "FlxState.h"
#include "FlxBasic.h"
#include "FlxCamera.h"
#include "tweens/FlxTween.h"
#include "util/FlxTimer.h"
#include <SDL_mixer.h>
#include <stdexcept>
#include <iostream>

namespace flixel {

bool FlxGame::muted = false;
bool FlxGame::zeroKeyPressed = false;

FlxGame::FlxGame(int gameWidth, int gameHeight, int updateFramerate, int drawFramerate, const std::string& title)
    : currentState(nullptr)
    , nextState(nullptr)
    , updateFramerate(updateFramerate)
    , drawFramerate(drawFramerate)
    , width(gameWidth)
    , height(gameHeight)
    , running(false)
    , fullscreen(false)
    , windowTitle(title)
{
    stepMS = 1000.0f / updateFramerate;
    stepSeconds = stepMS / 1000.0f;
    accumulator = stepMS;
    maxAccumulation = 2000.0f / drawFramerate - 1.0f;

    if (maxAccumulation < stepMS) {
        maxAccumulation = stepMS;
    }

    FlxG::init(this, width, height);
    tweens::init();
}

FlxGame::~FlxGame() {
    if (currentState) {
        delete currentState;
    }
    tweens::cleanup();
    FlxG::destroy();
}

void FlxGame::run() {
    running = true;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {}
    Uint32 lastTime = SDL_GetTicks();
    Uint32 currentTime;
    float deltaTime;

    while (running) {
        currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        handleEvents();

        if (FlxG::fixedTimestep) {
            accumulator += deltaTime * 1000.0f;
            if (accumulator > maxAccumulation) {
                accumulator = maxAccumulation;
            }

        while (accumulator >= stepMS) {
            flixel::FlxG::keys.update();
            flixel::FlxG::gamepads.update();
            update(stepSeconds);
            accumulator -= stepMS;
        }
    } else {
        flixel::FlxG::keys.update();
        flixel::FlxG::gamepads.update();
        update(deltaTime);
    }

        draw();
    }
}

void FlxGame::update(float elapsed) {
    const Uint8* keystate = SDL_GetKeyboardState(nullptr);
    if (keystate[SDL_SCANCODE_0]) {
        if (!zeroKeyPressed) {
            toggleMute();
            zeroKeyPressed = true;
        }
    } else {
        zeroKeyPressed = false;
    }
    
    if (muted) {
        Mix_Volume(-1, 0);
        Mix_VolumeMusic(0);
    }
    
    if (nextState) {
        if (currentState) {
            currentState->destroy();
            delete currentState;
        }
        currentState = nextState;
        nextState = nullptr;
        FlxG::keys.reset();
        FlxG::gamepads.reset();
        fprintf(stderr, "[UPDATE] calling state create()\n"); fflush(stderr);
        currentState->create();
        fprintf(stderr, "[UPDATE] create() returned\n"); fflush(stderr);
    }

    if (!currentState) {
        return;
    }

    currentState->update(elapsed);
    
    if (FlxG::camera) {
        FlxG::camera->update(elapsed);
    }
    
    if (tweens::globalManager) {
        tweens::globalManager->update(elapsed);
    }
    
    if (FlxG::timers) {
        FlxG::timers->update(elapsed);
    }

    if (overlay) {
        overlay->update(elapsed);
    }
}

void FlxGame::draw() {
    if (!currentState) {
        return;
    }

    SDL_SetRenderDrawColor(FlxG::renderer, 0, 0, 0, 255);
    SDL_RenderClear(FlxG::renderer);

    currentState->draw();

    if (FlxG::camera) {
        FlxG::camera->drawFX();
    }

    if (overlay) {
        overlay->draw();
    }

    SDL_RenderPresent(FlxG::renderer);
}

void FlxGame::switchState(FlxState* newState) {
    nextState = newState;
}

void FlxGame::onResize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    FlxG::resizeGame(width, height);
}

void FlxGame::setFullscreen(bool value) {
    fullscreen = value;
    FlxG::setFullscreen(value);
}

void FlxGame::setUpdateFramerate(int fps) {
    updateFramerate = fps;
    stepMS = 1000.0f / fps;
    stepSeconds = stepMS / 1000.0f;
}

void FlxGame::setDrawFramerate(int fps) {
    drawFramerate = fps;
    maxAccumulation = 2000.0f / fps - 1.0f;
    if (maxAccumulation < stepMS) {
        maxAccumulation = stepMS;
    }
}

void FlxGame::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        flixel::FlxG::keys.onEvent(event);
        flixel::FlxG::gamepads.onEvent(event);

        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    onResize(event.window.data1, event.window.data2);
                }
                break;

            case SDL_KEYDOWN:
                break;
        }
    }
}

void FlxGame::calculateElapsed() {
    if (FlxG::fixedTimestep) {
        FlxG::elapsed = FlxG::timeScale * stepSeconds;
    } else {
        FlxG::elapsed = FlxG::timeScale * (accumulator / 1000.0f);
        if (FlxG::elapsed > FlxG::maxElapsed * FlxG::timeScale) {
            FlxG::elapsed = FlxG::maxElapsed * FlxG::timeScale;
        }
    }
}

void FlxGame::setTitle(const std::string& title) {
    windowTitle = title;
    if (FlxG::window) {
        SDL_SetWindowTitle(FlxG::window, title.c_str());
    }
}

void FlxGame::toggleMute() {
    muted = !muted;
    
    if (muted) {
        Mix_Volume(-1, 0);
        Mix_VolumeMusic(0);
        std::cout << "[Audio] Muted all audio" << std::endl;
    } else {
        Mix_Volume(-1, MIX_MAX_VOLUME);
        Mix_VolumeMusic(MIX_MAX_VOLUME);
        std::cout << "[Audio] Unmuted all audio" << std::endl;
    }
}
}
