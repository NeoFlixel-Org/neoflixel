#pragma once

#include "../FlxBasic.h"
#include "../FlxG.h"
#include "../math/FlxPoint.h"
#include <string>
#include <SDL2/SDL_mixer.h>
#include <vorbis/vorbisfile.h>
#include <functional>

namespace flixel {

class FlxObject;
class FlxSoundGroup;

class FlxSound : public FlxBasic {
    friend class FlxSoundGroup;

public:
    FlxSound();
    ~FlxSound();

    void kill() override;
    void revive() override;
    void update(float elapsed) override;

    bool loadEmbedded(const std::string& path, bool looped = false, bool autoDestroy = true);
    bool loadStream(const std::string& path, bool looped = false, bool autoDestroy = false);
    bool loadByteArray(const void* data, size_t size, bool looped = false, bool autoDestroy = false);

    void play(bool forceRestart = false, float startTime = 0.0f, float endTime = 0.0f);
    void pause();
    void resume();
    void stop();
    void fadeOut(float duration = 1.0f, float to = 0.0f);
    void fadeIn(float duration = 1.0f, float from = 0.0f, float to = 1.0f);

    void setPosition(float x, float y);
    void proximity(float x, float y, FlxObject* target, float radius, bool pan = true);
    float getActualVolume() const;

    float get_volume() const;
    void setVolume(float volume);
    bool get_muted() const;
    void set_muted(bool value);
    float get_pan() const;
    void set_pan(float pan);
    float get_time() const;
    void set_time(float time);
    float get_length() const;
    bool get_looped() const;
    void set_looped(bool value);
    float get_loopTime() const;
    void set_loopTime(float value);
    float get_endTime() const;
    void set_endTime(float value);
    bool get_playing() const;

    float x;
    float y;
    bool persist;
    std::string name;
    std::string artist;
    float amplitude;
    float amplitudeLeft;
    float amplitudeRight;
    bool autoDestroy;
    std::function<void()> onComplete;
    bool exists;
    bool active;
    bool playing;
    bool paused;
    bool looped;
    bool muted;
    FlxSoundGroup* group;

protected:
    Mix_Chunk* chunk;
    Mix_Music* music;
    OggVorbis_File vorbisFile;
    bool isStream;
    bool isVorbis;

    float _volume;
    float _time;
    float _length;
    float _volumeAdjust;
    bool _proximityPan;
    bool _alreadyPaused;
    bool _paused;

    float _pan;
    float _radius;
    FlxObject* _target;
    FlxPoint scrollFactor;

    float fadeStartVolume;
    float fadeTargetVolume;
    float fadeDuration;
    float fadeTime;
    bool isFading;

    int channel;
    bool _muted;

    void updateTransform();
    void cleanup(bool destroySound, bool resetPosition = true);
    float calcTransformVolume() const;
    void startSound(float startTime);
    void stopped();
    void gotID3();

private:
    void destroy();
    void reset();
};
}