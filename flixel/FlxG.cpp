#include "FlxG.h"
#include "FlxGame.h"
#include "sound/FlxSound.h"
#include "sound/FlxSoundGroup.h"
#include "input/FlxGamepad.h"
#include "util/FlxTimer.h"
#include <stdexcept>
#include <iostream>
#ifdef __vita__
#include <vitaGL.h>
#endif

namespace flixel {

bool FlxG::autoPause = true;
bool FlxG::fixedTimestep = true;
float FlxG::timeScale = 1.0f;
float FlxG::animationTimeScale = 1.0f;

FlxGame* FlxG::game = nullptr;
SDL_Window* FlxG::window = nullptr;
SDL_Renderer* FlxG::renderer = nullptr;
FlxCamera* FlxG::camera = nullptr;

float FlxG::elapsed = 0.0f;
float FlxG::maxElapsed = 0.1f;

bool FlxG::initialized = false;

int FlxG::width = 0;
int FlxG::height = 0;
int FlxG::initialWidth = 0;
int FlxG::initialHeight = 0;

FlxG::WorldBounds FlxG::worldBounds = {0, 0, 0, 0};
FlxG::SoundFrontEnd FlxG::sound;
FlxG::Log FlxG::log;

SDL_Cursor* FlxG::customCursor = nullptr;
SDL_Surface* FlxG::cursorSurface = nullptr;
bool FlxG::cursorVisible = true;
std::unordered_map<std::string, SDL_Texture*> FlxG::textureCache;

flixel::input::FlxKeyboard flixel::FlxG::keys;
flixel::input::FlxGamepad flixel::FlxG::gamepads;
flixel::util::FlxTimerManager* flixel::FlxG::timers = nullptr;

void FlxG::Log::error(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

void FlxG::Log::warn(const std::string& message) {
    std::cerr << "[WARN] " << message << std::endl;
}

void FlxG::Log::notice(const std::string& message) {
    std::cout << "[NOTICE] " << message << std::endl;
}

FlxG::SoundFrontEnd::SoundFrontEnd() : volume(1.0f), muted(false), music(nullptr) {
    reset();
}

FlxG::SoundFrontEnd::~SoundFrontEnd() {
    destroy();
}

void FlxG::SoundFrontEnd::destroy() {
    if (music) {
        music->stop();
        music.reset();
    }
    stopAll();
    sounds.clear();
    groups.clear();
}

void FlxG::SoundFrontEnd::update(float elapsed) {
    for (auto& sound : sounds) {
        if (sound->active) {
            sound->update(elapsed);
        }
    }

    sounds.erase(
        std::remove_if(sounds.begin(), sounds.end(),
            [](const std::unique_ptr<FlxSound>& sound) {
                return !sound->exists;
            }),
        sounds.end()
    );
}

void FlxG::SoundFrontEnd::reset() {
    volume = 1.0f;
    muted = false;
    stopAll();
    sounds.clear();
    groups.clear();
    defaultGroup = "default";
    addGroup(defaultGroup);
}

FlxSound* FlxG::SoundFrontEnd::load(const std::string& path, bool looped, bool autoDestroy) {
    auto sound = std::make_unique<FlxSound>();
    if (sound->loadEmbedded(path, looped, autoDestroy)) {
        sounds.push_back(std::move(sound));
        return sounds.back().get();
    }
    return nullptr;
}

FlxSound* FlxG::SoundFrontEnd::play(const std::string& path, float volume, bool looped, bool autoDestroy) {
    FlxSound* sound = load(path, looped, autoDestroy);
    if (sound) {
        sound->setVolume(volume);
        sound->play();
    }
    return sound;
}

FlxSound* FlxG::SoundFrontEnd::playAsChunk(const std::string& path, float volume, bool looped, bool autoDestroy) {
    auto sound = std::make_unique<FlxSound>();
    if (sound->loadAsChunk(path, looped, autoDestroy)) {
        sound->setVolume(volume);
        sound->play();
        sounds.push_back(std::move(sound));
        return sounds.back().get();
    }
    return nullptr;
}

FlxSound* FlxG::SoundFrontEnd::playMusic(const std::string& path, float volume, bool looped) {
    if (music) {
        music->stop();
    }
    
    music = std::make_unique<FlxSound>();
    if (music->loadStream(path, looped, false)) {
        music->setVolume(volume);
        music->play();
        return music.get();
    }
    
    music.reset();
    return nullptr;
}

void FlxG::SoundFrontEnd::stop(const std::string& path) {
    for (auto& sound : sounds) {
        if (sound->exists && sound->name == path) {
            sound->stop();
        }
    }
}

void FlxG::SoundFrontEnd::pause(const std::string& path) {
    for (auto& sound : sounds) {
        if (sound->exists && sound->name == path) {
            sound->pause();
        }
    }
}

void FlxG::SoundFrontEnd::resume(const std::string& path) {
    for (auto& sound : sounds) {
        if (sound->exists && sound->name == path) {
            sound->resume();
        }
    }
}

void FlxG::SoundFrontEnd::stopAll() {
    for (auto& sound : sounds) {
        if (sound->exists) {
            sound->stop();
        }
    }
}

void FlxG::SoundFrontEnd::pauseAll() {
    for (auto& sound : sounds) {
        if (sound->exists) {
            sound->pause();
        }
    }
}

void FlxG::SoundFrontEnd::resumeAll() {
    for (auto& sound : sounds) {
        if (sound->exists) {
            sound->resume();
        }
    }
}

FlxSoundGroup* FlxG::SoundFrontEnd::addGroup(const std::string& name, float volume) {
    auto group = std::make_unique<FlxSoundGroup>(name, volume);
    groups.push_back(std::move(group));
    return groups.back().get();
}

FlxSoundGroup* FlxG::SoundFrontEnd::getGroup(const std::string& name) {
    for (auto& group : groups) {
        if (group->name == name) {
            return group.get();
        }
    }
    return nullptr;
}

void FlxG::SoundFrontEnd::removeGroup(const std::string& name) {
    groups.erase(
        std::remove_if(groups.begin(), groups.end(),
            [&name](const std::unique_ptr<FlxSoundGroup>& group) {
                return group->name == name;
            }),
        groups.end()
    );
}

void FlxG::init(FlxGame* gameInstance, int gameWidth, int gameHeight) {
    if (initialized) {
        return;
    }

    game = gameInstance;
    width = gameWidth;
    height = gameHeight;
    initialWidth = gameWidth;
    initialHeight = gameHeight;

#ifdef __vita__
    vglInitExtended(0, 960, 544, 0x1800000, SCE_GXM_MULTISAMPLE_NONE);
#endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        throw std::runtime_error("Failed to initialize SDL_image: " + std::string(IMG_GetError()));
    }

    int mixFlags = MIX_INIT_OGG;
    if (!(Mix_Init(mixFlags) & MIX_INIT_OGG)) {
        throw std::runtime_error("Failed to initialize SDL_mixer (OGG): " + std::string(Mix_GetError()));
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        throw std::runtime_error("Failed to open audio device: " + std::string(Mix_GetError()));
    }

    Mix_AllocateChannels(32);
    Mix_ReserveChannels(1);

    if (TTF_Init() < 0) {
        throw std::runtime_error("Failed to initialize SDL_ttf: " + std::string(TTF_GetError()));
    }

    window = SDL_CreateWindow(
        game->windowTitle.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        throw std::runtime_error("Failed to create window: " + std::string(SDL_GetError()));
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        throw std::runtime_error("Failed to create renderer: " + std::string(SDL_GetError()));
    }
    
    SDL_RenderSetLogicalSize(renderer, width, height);
    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

    try {
        setCursor(ASSETS_PATH "assets/images/ui/cursor.png", 0, 0);
    }
    catch (const std::exception& e) {
        log.warn("Failed to load default cursor: " + std::string(e.what()));
        setDefaultCursor();
    }

    worldBounds = {0, 0, static_cast<float>(width), static_cast<float>(height)};

    sound.reset();
    gamepads.init();

    fprintf(stderr, "[CHECKPOINT] gamepads done\n"); fflush(stderr);
    
    if (!timers) {
        timers = new util::FlxTimerManager();
    }

    fprintf(stderr, "[CHECKPOINT] timers done\n"); fflush(stderr);

    initialized = true;
    fprintf(stderr, "[CHECKPOINT] FlxG::init done\n"); fflush(stderr);
}

void FlxG::reset() {
    autoPause = true;
    fixedTimestep = true;
    timeScale = 1.0f;
    animationTimeScale = 1.0f;
    elapsed = 0.0f;
    maxElapsed = 0.1f;
    sound.reset();
}

void FlxG::resizeGame(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    worldBounds = {0, 0, static_cast<float>(newWidth), static_cast<float>(newHeight)};

    if (renderer) {
        SDL_RenderSetLogicalSize(renderer, newWidth, newHeight);
    }
}

void FlxG::resizeWindow(int newWidth, int newHeight) {
    SDL_SetWindowSize(window, newWidth, newHeight);
}

void FlxG::setFullscreen(bool fullscreen) {
    if (fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(window, 0);
    }
}

SDL_Texture* FlxG::loadTexture(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        fprintf(stderr, "[TEXTURE] IMG_Load failed for %s: %s\n", path.c_str(), IMG_GetError());
        fflush(stderr);
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        fprintf(stderr, "[TEXTURE] CreateTextureFromSurface failed for %s: %s\n", path.c_str(), SDL_GetError());
        fflush(stderr);
        return nullptr;
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return texture;
}

SDL_Texture* FlxG::loadTextureCached(const std::string& path) {
    auto it = textureCache.find(path);
    if (it != textureCache.end()) {
        return it->second;
    }

    SDL_Texture* texture = loadTexture(path);
    
    textureCache[path] = texture;
    
    return texture;
}

void FlxG::clearTextureCache() {
    for (auto& pair : textureCache) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textureCache.clear();
}

Mix_Chunk* FlxG::loadSound(const std::string& path) {
    Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
    if (!sound) {
        throw std::runtime_error("Failed to load sound: " + std::string(Mix_GetError()));
    }
    return sound;
}

TTF_Font* FlxG::loadFont(const std::string& path, int size) {
    TTF_Font* font = TTF_OpenFont(path.c_str(), size);
    if (!font) {
        throw std::runtime_error("Failed to load font: " + std::string(TTF_GetError()));
    }
    return font;
}

void FlxG::setCursor(const std::string& path, int hotX, int hotY) {
    if (customCursor) {
        SDL_FreeCursor(customCursor);
        customCursor = nullptr;
    }
    if (cursorSurface) {
        SDL_FreeSurface(cursorSurface);
        cursorSurface = nullptr;
    }

    cursorSurface = IMG_Load(path.c_str());
    if (!cursorSurface) {
        log.error("Failed to load cursor image: " + std::string(IMG_GetError()));
        setDefaultCursor();
        return;
    }

    customCursor = SDL_CreateColorCursor(cursorSurface, hotX, hotY);
    if (!customCursor) {
        log.error("Failed to create cursor: " + std::string(SDL_GetError()));
        SDL_FreeSurface(cursorSurface);
        cursorSurface = nullptr;
        setDefaultCursor();
        return;
    }

    SDL_SetCursor(customCursor);
    cursorVisible = true;
}

void FlxG::setDefaultCursor() {
    if (customCursor) {
        SDL_FreeCursor(customCursor);
        customCursor = nullptr;
    }
    if (cursorSurface) {
        SDL_FreeSurface(cursorSurface);
        cursorSurface = nullptr;
    }
    SDL_SetCursor(SDL_GetDefaultCursor());
}

void FlxG::showCursor(bool show) {
    SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
    cursorVisible = show;
}

void FlxG::setCursorVisible(bool visible) {
    showCursor(visible);
}

bool FlxG::isCursorVisible() {
    return cursorVisible;
}

void FlxG::destroy() {
    if (!initialized) {
        return;
    }

    clearTextureCache();
    
    if (timers) {
        delete timers;
        timers = nullptr;
    }
    
    gamepads.close();

    if (customCursor) {
        SDL_FreeCursor(customCursor);
        customCursor = nullptr;
    }
    if (cursorSurface) {
        SDL_FreeSurface(cursorSurface);
        cursorSurface = nullptr;
    }

    sound.destroy();

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

    initialized = false;
}
}