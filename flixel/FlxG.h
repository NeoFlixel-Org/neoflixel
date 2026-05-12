#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "../imports.h"
#include "math/FlxRect.h"
#include "FlxCamera.h"
#include <vector>
#include <memory>
#include "sound/FlxSound.h"
#include "sound/FlxSoundGroup.h"
#include "input/FlxKeyboard.h"

namespace flixel {

class FlxGame;

namespace system {
namespace frontEnds {

class SoundFrontEnd {
public:
    SoundFrontEnd();
    ~SoundFrontEnd();

    void destroy();
    void update(float elapsed);
    void reset();

    FlxSound* load(const std::string& path, bool looped = false, bool autoDestroy = true);
    FlxSound* play(const std::string& path, float volume = 1.0f, bool looped = false, bool autoDestroy = true);
    void stop(const std::string& path);
    void pause(const std::string& path);
    void resume(const std::string& path);
    void stopAll();
    void pauseAll();
    void resumeAll();

    FlxSoundGroup* addGroup(const std::string& name, float volume = 1.0f);
    FlxSoundGroup* getGroup(const std::string& name);
    void removeGroup(const std::string& name);

    float volume;
    bool muted;
    std::string defaultGroup;

private:
    std::vector<std::unique_ptr<FlxSound>> sounds;
    std::vector<std::unique_ptr<FlxSoundGroup>> groups;
};

}
}

class FlxG {
public:
    static bool autoPause;
    static bool fixedTimestep;
    static float timeScale;
    static float animationTimeScale;

    static FlxGame* game;
    static SDL_Window* window;
    static SDL_Renderer* renderer;
    static FlxCamera* camera;

    static float elapsed;
    static float maxElapsed;

    static bool initialized;

    static int width;
    static int height;
    static int initialWidth;
    static int initialHeight;

    struct WorldBounds {
        float x;
        float y;
        float width;
        float height;

        bool containsXY(float x, float y) const {
            return x >= this->x && x < this->x + width &&
                   y >= this->y && y < this->y + height;
        }
    };

    class SoundFrontEnd {
    public:
        SoundFrontEnd();
        ~SoundFrontEnd();

        void destroy();
        void update(float elapsed);
        void reset();

        FlxSound* load(const std::string& path, bool looped = false, bool autoDestroy = true);
        FlxSound* play(const std::string& path, float volume = 1.0f, bool looped = false, bool autoDestroy = true);
        void stop(const std::string& path);
        void pause(const std::string& path);
        void resume(const std::string& path);
        void stopAll();
        void pauseAll();
        void resumeAll();

        FlxSoundGroup* addGroup(const std::string& name, float volume = 1.0f);
        FlxSoundGroup* getGroup(const std::string& name);
        void removeGroup(const std::string& name);

        float volume;
        bool muted;
        std::string defaultGroup;

    private:
        std::vector<std::unique_ptr<FlxSound>> sounds;
        std::vector<std::unique_ptr<FlxSoundGroup>> groups;
    };

    class Log {
    public:
        void error(const std::string& message);
        void warn(const std::string& message);
        void notice(const std::string& message);
    };

    static WorldBounds worldBounds;
    static SoundFrontEnd sound;
    static Log log;

    static void init(FlxGame* gameInstance, int gameWidth, int gameHeight);
    static void reset();
    static void destroy();

    static void resizeGame(int width, int height);
    static void resizeWindow(int width, int height);
    static void setFullscreen(bool fullscreen);
    
    static SDL_Texture* loadTexture(const std::string& path);
    static Mix_Chunk* loadSound(const std::string& path);
    static TTF_Font* loadFont(const std::string& path, int size);

    static void setCursor(const std::string& path, int hotX = 0, int hotY = 0);
    static void setDefaultCursor();
    static void showCursor(bool show);
    static void setCursorVisible(bool visible);
    static bool isCursorVisible();

    static flixel::input::FlxKeyboard keys;

private:
    static SDL_Cursor* customCursor;
    static SDL_Surface* cursorSurface;
    static bool cursorVisible;
};
}