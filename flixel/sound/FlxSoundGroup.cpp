#include "FlxSoundGroup.h"
#include "FlxSound.h"
#include <algorithm>

namespace flixel {

FlxSoundGroup::FlxSoundGroup(const std::string& name, float volume)
    : name(name)
    , volume(volume)
    , muted(false)
{
}

FlxSoundGroup::~FlxSoundGroup()
{
    while (!sounds.empty()) {
        remove(sounds.back());
    }
}

bool FlxSoundGroup::add(FlxSound* sound)
{
    if (!sound) {
        return false;
    }

    if (std::find(sounds.begin(), sounds.end(), sound) != sounds.end()) {
        return false;
    }

    if (sound->group != nullptr) {
        sound->group->remove(sound);
    }

    sounds.push_back(sound);
    sound->group = this;
    sound->updateTransform();
    return true;
}

bool FlxSoundGroup::remove(FlxSound* sound)
{
    if (!sound) {
        return false;
    }

    auto it = std::find(sounds.begin(), sounds.end(), sound);
    if (it != sounds.end()) {
        sound->group = nullptr;
        sounds.erase(it);
        sound->updateTransform();
        return true;
    }
    return false;
}

void FlxSoundGroup::pause()
{
    for (auto* sound : sounds) {
        if (sound && sound->exists) {
            sound->pause();
        }
    }
}

void FlxSoundGroup::resume()
{
    for (auto* sound : sounds) {
        if (sound && sound->exists) {
            sound->resume();
        }
    }
}

float FlxSoundGroup::getVolume() const
{
    return muted ? 0.0f : volume;
}

float FlxSoundGroup::get_volume() const
{
    return volume;
}

void FlxSoundGroup::set_volume(float newVolume)
{
    volume = newVolume;
    for (auto* sound : sounds) {
        if (sound && sound->exists) {
            sound->updateTransform();
        }
    }
}

bool FlxSoundGroup::get_muted() const
{
    return muted;
}

void FlxSoundGroup::set_muted(bool value)
{
    muted = value;
    for (auto* sound : sounds) {
        if (sound && sound->exists) {
            sound->updateTransform();
        }
    }
}

}