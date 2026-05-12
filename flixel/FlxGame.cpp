#include "FlxGame.h"
#include "FlxG.h"
#include "FlxState.h"
#include <stdexcept>

namespace flixel {

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
}

FlxGame::~FlxGame() {
    if (currentState) {
        delete currentState;
    }
    FlxG::destroy();
}

void FlxGame::run() {
    running = true;
    Uint32 lastTime = SDL_GetTicks();
    Uint32 currentTime;
    float deltaTime;

    while (running) {
        currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        handleEvents();

        flixel::FlxG::keys.update();

        if (FlxG::fixedTimestep) {
            accumulator += deltaTime * 1000.0f;
            if (accumulator > maxAccumulation) {
                accumulator = maxAccumulation;
            }

            while (accumulator >= stepMS) {
                update(stepSeconds);
                accumulator -= stepMS;
            }
        } else {
            update(deltaTime);
        }

        draw();
    }
}

void FlxGame::update(float elapsed) {
    if (nextState) {
        if (currentState) {
            currentState->destroy();
            delete currentState;
        }
        currentState = nextState;
        nextState = nullptr;
        currentState->create();
    }

    if (!currentState) {
        return;
    }

    currentState->update(elapsed);
}

void FlxGame::draw() {
    if (!currentState) {
        return;
    }

    SDL_SetRenderDrawColor(FlxG::renderer, 0, 0, 0, 255);
    SDL_RenderClear(FlxG::renderer);

    currentState->draw();

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
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
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
}