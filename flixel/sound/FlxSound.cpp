#include "FlxSound.h"
#include "../FlxG.h"
#include "../math/FlxMath.h"
#include <SDL2/SDL.h>
#include <algorithm>

namespace flixel {

FlxSound::FlxSound() : 
    chunk(nullptr),
    music(nullptr),
    isStream(false),
    isVorbis(false),
    _paused(false),
    _volume(1.0f),
    _time(0.0f),
    _length(0.0f),
    _volumeAdjust(1.0f),
    _proximityPan(false),
    _alreadyPaused(false),
    _pan(0.0f),
    _radius(0.0f),
    _target(nullptr),
    fadeStartVolume(0.0f),
    fadeTargetVolume(0.0f),
    fadeDuration(0.0f),
    fadeTime(0.0f),
    isFading(false),
    looped(false)
{
    reset();
}

FlxSound::~FlxSound()
{
    destroy();
}

void FlxSound::reset()
{
    cleanup(true, true);
    
    x = 0;
    y = 0;
    _time = 0;
    _paused = false;
    _volume = 1.0f;
    _volumeAdjust = 1.0f;
    looped = false;
    set_loopTime(0.0f);
    set_endTime(0.0f);
    _target = nullptr;
    _radius = 0;
    _proximityPan = false;
    visible = false;
    amplitude = 0;
    amplitudeLeft = 0;
    amplitudeRight = 0;
    autoDestroy = false;
    _pan = 0;
}

void FlxSound::destroy()
{
    cleanup(true, true);
    exists = false;
    active = false;
    _target = nullptr;
    name.clear();
    artist.clear();
    onComplete = nullptr;
}

void FlxSound::update(float elapsed)
{
    if (!get_playing())
        return;

    if (isFading)
    {
        fadeTime += elapsed;
        float progress = std::min(fadeTime / fadeDuration, 1.0f);
        float newVolume = fadeStartVolume + (fadeTargetVolume - fadeStartVolume) * progress;
        setVolume(newVolume);

        if (progress >= 1.0f)
        {
            isFading = false;
            if (fadeTargetVolume <= 0.0f)
                stop();
        }
    }

    if (isStream)
        _time = Mix_GetMusicPosition(music) * 1000.0f;
    else if (chunk)
        _time += elapsed * 1000.0f;

    float radialMultiplier = 1.0f;
    if (_target != nullptr)
    {
        FlxPoint targetPos(_target->x, _target->y);
        float distance = targetPos.distanceTo(FlxPoint(x, y));
        radialMultiplier = distance / _radius;
        radialMultiplier = 1.0f - math::bound(radialMultiplier, 0.0f, 1.0f);

        if (_proximityPan)
        {
            float d = (x - _target->x) / _radius;
            set_pan(math::bound(d, -1.0f, 1.0f));
        }
    }

    _volumeAdjust = radialMultiplier;
    updateTransform();

    if (_volume > 0)
    {
        amplitudeLeft = _volume * 0.5f;
        amplitudeRight = _volume * 0.5f;
        amplitude = (amplitudeLeft + amplitudeRight) * 0.5f;
    }
    else
    {
        amplitudeLeft = 0;
        amplitudeRight = 0;
        amplitude = 0;
    }

    if (get_endTime() > 0 && _time >= get_endTime())
        stopped();
}

void FlxSound::kill()
{
    FlxBasic::kill();
    cleanup(false);
}

void FlxSound::revive()
{
    FlxBasic::revive();
}

bool FlxSound::loadEmbedded(const std::string& path, bool looped, bool autoDestroy)
{
    cleanup(true);
    
    music = Mix_LoadMUS(path.c_str());
    if (music)
    {
        isStream = true;
        isVorbis = path.find(".ogg") != std::string::npos;
    }
    else
    {
        chunk = Mix_LoadWAV(path.c_str());
        if (!chunk)
        {
            FlxG::log.error("Could not load sound: " + path);
            return false;
        }
        isStream = false;
    }

    set_looped(looped);
    this->autoDestroy = autoDestroy;
    updateTransform();
    exists = true;
    
    if (isStream)
        _length = Mix_MusicDuration(music) * 1000.0f;
    else
        _length = chunk->alen * 1000.0f / 44100.0f;
    
    set_endTime(_length);
    return true;
}

bool FlxSound::loadStream(const std::string& path, bool looped, bool autoDestroy)
{
    return loadEmbedded(path, looped, autoDestroy);
}

bool FlxSound::loadByteArray(const void* data, size_t size, bool looped, bool autoDestroy)
{
    cleanup(true);
    
    SDL_RWops* rw = SDL_RWFromConstMem(data, size);
    if (!rw)
    {
        FlxG::log.error("Could not create RWops from memory");
        return false;
    }

    music = Mix_LoadMUS_RW(rw, 1);
    if (music)
    {
        isStream = true;
        isVorbis = true;
    }
    else
    {
        chunk = Mix_LoadWAV_RW(rw, 1);
        if (!chunk)
        {
            FlxG::log.error("Could not load sound from memory");
            return false;
        }
        isStream = false;
    }

    set_looped(looped);
    this->autoDestroy = autoDestroy;
    updateTransform();
    exists = true;
    
    if (isStream)
        _length = Mix_MusicDuration(music) * 1000.0f;
    else
        _length = chunk->alen * 1000.0f / 44100.0f;
    
    set_endTime(_length);
    return true;
}

void FlxSound::play(bool forceRestart, float startTime, float endTime)
{
    if (!exists)
        return;

    if (forceRestart)
        cleanup(false, true);
    else if (get_playing())
        return;

    if (_paused)
        resume();
    else
        startSound(startTime);

    set_endTime(endTime);
}

void FlxSound::pause()
{
    if (!get_playing())
        return;

    _time = get_time();
    _paused = true;
    
    if (isStream)
        Mix_PauseMusic();
    else
        Mix_Pause(-1);
}

void FlxSound::resume()
{
    if (_paused)
    {
        if (isStream)
            Mix_ResumeMusic();
        else
            Mix_Resume(-1);
        _paused = false;
    }
}

void FlxSound::stop()
{
    cleanup(autoDestroy, true);
}

void FlxSound::fadeOut(float duration, float to)
{
    if (isFading)
        return;

    fadeStartVolume = get_volume();
    fadeTargetVolume = to;
    fadeDuration = duration;
    fadeTime = 0;
    isFading = true;
}

void FlxSound::fadeIn(float duration, float from, float to)
{
    if (!get_playing())
        play();

    if (isFading)
        return;

    setVolume(from);
    fadeStartVolume = from;
    fadeTargetVolume = to;
    fadeDuration = duration;
    fadeTime = 0;
    isFading = true;
}

void FlxSound::setPosition(float x, float y)
{
    this->x = x;
    this->y = y;
}

void FlxSound::proximity(float x, float y, FlxObject* target, float radius, bool pan)
{
    this->x = x;
    this->y = y;
    _target = target;
    _radius = radius;
    _proximityPan = pan;
}

float FlxSound::getActualVolume() const
{
    return _volume * _volumeAdjust;
}

bool FlxSound::get_playing() const
{
    if (isStream)
        return Mix_PlayingMusic() != 0;
    else
        return Mix_Playing(-1) != 0;
}

float FlxSound::get_volume() const
{
    return _volume;
}

void FlxSound::setVolume(float volume)
{
    _volume = math::bound(volume, 0.0f, 1.0f);
    updateTransform();
}

float FlxSound::get_pan() const
{
    return _pan;
}

void FlxSound::set_pan(float pan)
{
    _pan = math::bound(pan, -1.0f, 1.0f);
    updateTransform();
}

float FlxSound::get_time() const
{
    return _time;
}

void FlxSound::set_time(float time)
{
    if (get_playing())
    {
        cleanup(false, true);
        startSound(time);
    }
    _time = time;
}

float FlxSound::get_length() const
{
    return _length;
}

bool FlxSound::get_looped() const
{
    return looped;
}

void FlxSound::set_looped(bool value)
{
    looped = value;
    if (isStream && music)
    {
        Mix_PlayMusic(music, value ? -1 : 1);
    }
}

float FlxSound::get_loopTime() const
{
    return _time;
}

void FlxSound::set_loopTime(float value)
{
    _time = value;
}

float FlxSound::get_endTime() const
{
    return _length;
}

void FlxSound::set_endTime(float value)
{
    _length = value;
}

void FlxSound::cleanup(bool destroySound, bool resetPosition)
{
    if (destroySound)
    {
        x = 0;
        y = 0;
        _time = 0;
        _paused = false;
        _volume = 1.0f;
        _volumeAdjust = 1.0f;
        looped = false;
        set_loopTime(0.0f);
        set_endTime(0.0f);
        _target = nullptr;
        _radius = 0;
        _proximityPan = false;
        visible = false;
        amplitude = 0;
        amplitudeLeft = 0;
        amplitudeRight = 0;
        autoDestroy = false;
        _pan = 0;

        if (isStream)
        {
            Mix_HaltMusic();
            Mix_FreeMusic(music);
            music = nullptr;
        }
        else if (chunk)
        {
            Mix_HaltChannel(-1);
            Mix_FreeChunk(chunk);
            chunk = nullptr;
        }
    }
    else
    {
        if (isStream)
        {
            Mix_HaltMusic();
        }
        else if (chunk)
        {
            Mix_HaltChannel(-1);
        }
    }

    active = false;

    if (resetPosition)
    {
        _time = 0;
        _paused = false;
    }
}

void FlxSound::updateTransform()
{
    float volume = calcTransformVolume();
    
    if (isStream)
        Mix_VolumeMusic(static_cast<int>(volume * MIX_MAX_VOLUME));
    else if (chunk)
        Mix_VolumeChunk(chunk, static_cast<int>(volume * MIX_MAX_VOLUME));

    if (!isStream && chunk)
    {
        int leftVol = static_cast<int>(volume * MIX_MAX_VOLUME * (1.0f - _pan));
        int rightVol = static_cast<int>(volume * MIX_MAX_VOLUME * (1.0f + _pan));
        Mix_SetPanning(-1, leftVol, rightVol);
    }
}

float FlxSound::calcTransformVolume() const
{
    float volume = _volume * _volumeAdjust;
    
    if (FlxG::sound.muted)
        return 0.0f;
    
    return FlxG::sound.volume * volume;
}

void FlxSound::startSound(float startTime)
{
    if (!chunk && !music)
        return;

    _time = startTime;
    _paused = false;

    if (isStream)
    {
        Mix_PlayMusic(music, get_looped() ? -1 : 0);
        Mix_SetMusicPosition(startTime / 1000.0f);
    }
    else
    {
        Mix_PlayChannel(-1, chunk, get_looped() ? -1 : 0);
    }

    active = true;
}

void FlxSound::stopped()
{
    if (onComplete)
        onComplete();

    if (get_looped())
    {
        cleanup(false);
        play(false, get_loopTime(), get_endTime());
    }
    else
        cleanup(autoDestroy);
}

void FlxSound::gotID3() {}
}