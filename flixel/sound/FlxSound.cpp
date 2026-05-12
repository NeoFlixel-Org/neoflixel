#include "FlxSound.h"
#include "../FlxG.h"
#include "../math/FlxMath.h"
#include <SDL.h>
#include <algorithm>

namespace {

float mixMusicDurationMs(Mix_Music* music)
{
#if SDL_MIXER_VERSION_ATLEAST(2, 6, 0)
    return static_cast<float>(Mix_MusicDuration(music) * 1000.0);
#else
    (void)music;
#  if defined(__SWITCH__) || defined(__vita__)
    return 180000.0f;
#  else
    return 0.0f;
#  endif
#endif
}

} // namespace

namespace flixel {

FlxSound::FlxSound() : 
    chunk(nullptr),
    music(nullptr),
    isStream(false),
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
    looped(false),
    channel(-1),
    reservedChannel(-1)
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
    channel = -1;
    reservedChannel = -1;
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
    bool wasPlaying = playing;
    bool isCurrentlyPlaying = get_playing();
    
    if (wasPlaying && !isCurrentlyPlaying)
    {
        playing = false;
        stopped();
        return;
    }
    
    if (!isCurrentlyPlaying)
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
    {
#ifdef __SWITCH__
        _time += elapsed * 1000.0f;
#else
        _time = Mix_GetMusicPosition(music) * 1000.0f;
#endif
    }
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
    }
    else
    {
#ifdef __SWITCH__
        SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
        if (rw) {
            chunk = Mix_LoadWAV_RW(rw, 1);
        } else {
            chunk = nullptr;
        }
#else
        chunk = Mix_LoadWAV(path.c_str());
#endif
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
        _length = mixMusicDurationMs(music);
    else
        _length = chunk->alen * 1000.0f / 44100.0f;
    
    set_endTime(_length);
    return true;
}

bool FlxSound::loadStream(const std::string& path, bool looped, bool autoDestroy)
{
    return loadEmbedded(path, looped, autoDestroy);
}

bool FlxSound::loadAsChunk(const std::string& path, bool looped, bool autoDestroy)
{
    cleanup(true);
    
#ifdef __SWITCH__
    SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
    if (rw) {
        chunk = Mix_LoadWAV_RW(rw, 1);
    } else {
        chunk = nullptr;
    }
#else
    chunk = Mix_LoadWAV(path.c_str());
#endif
    if (!chunk)
    {
        FlxG::log.error("Could not load sound as chunk: " + path);
        return false;
    }
    isStream = false;

    set_looped(looped);
    this->autoDestroy = autoDestroy;
    updateTransform();
    exists = true;
    
    _length = chunk->alen * 1000.0f / 44100.0f;
    set_endTime(_length);
    return true;
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
        _length = mixMusicDurationMs(music);
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
    if (paused)
        return;

    _time = get_time();
    _paused = true;
    paused = true;
    playing = false;
    
    if (isStream)
    {
        if (Mix_PlayingMusic())
            Mix_PauseMusic();
    }
    else if (channel >= 0)
    {
        if (Mix_Playing(channel))
            Mix_Pause(channel);
    }
}

void FlxSound::resume()
{
    if (!paused)
        return;
        
    if (isStream)
    {
        if (Mix_PausedMusic())
            Mix_ResumeMusic();
    }
    else if (channel >= 0)
    {
        if (Mix_Paused(channel))
            Mix_Resume(channel);
    }
    
    _paused = false;
    paused = false;
    playing = true;
}

void FlxSound::stop()
{
    if (isStream) {
        Mix_HaltMusic();
    } else if (channel >= 0) {
        Mix_HaltChannel(channel);
        channel = -1;
    }
    
    playing = false;
    paused = false;
    _paused = false;
    active = false;
    
    if (autoDestroy) {
        cleanup(true, true);
    }
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
    else if (channel >= 0)
        return Mix_Playing(channel) != 0;
    else
        return false;
}

float FlxSound::get_volume() const
{
    return _volume;
}

void FlxSound::setVolume(float volume)
{
    _volume = math::bound(volume, 0.0f, 1.0f);
    if (!isStream && channel >= 0) {
        Mix_Volume(channel, static_cast<int>(calcTransformVolume() * MIX_MAX_VOLUME));
    } else {
        updateTransform();
    }
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
            if (channel >= 0) {
                Mix_HaltChannel(channel);
                channel = -1;
            }
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
        else if (chunk && channel >= 0)
        {
            Mix_HaltChannel(channel);
            channel = -1;
        }
    }

    active = false;
    playing = false;
    paused = false;

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
    else if (channel >= 0)
        Mix_Volume(channel, static_cast<int>(volume * MIX_MAX_VOLUME));

    if (!isStream && channel >= 0 && chunk)
    {
        int leftVol = static_cast<int>(volume * MIX_MAX_VOLUME * (1.0f - _pan));
        int rightVol = static_cast<int>(volume * MIX_MAX_VOLUME * (1.0f + _pan));
        Mix_SetPanning(channel, leftVol, rightVol);
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
    paused = false;
    playing = true;

    if (isStream)
    {
        Mix_PlayMusic(music, get_looped() ? -1 : 0);
        Mix_SetMusicPosition(startTime / 1000.0f);
    }
    else
    {
        int channelToUse = (reservedChannel >= 0) ? reservedChannel : -1;
#ifdef __SWITCH__
        channel = Mix_PlayChannelTimed(channelToUse, chunk, get_looped() ? -1 : 0, -1);
#else
        channel = Mix_PlayChannel(channelToUse, chunk, get_looped() ? -1 : 0);
#endif
        if (channel == -1) {
            std::cerr << "Failed to play sound: " << Mix_GetError() << std::endl;
            playing = false;
        }
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

void FlxSound::setChannel(int channelToUse) {
    reservedChannel = channelToUse;
}

}