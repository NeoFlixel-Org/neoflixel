#pragma once

#include <string>
#include <vector>
#include <memory>

namespace flixel {

class FlxSound;

class FlxSoundGroup {
public:
    FlxSoundGroup(const std::string& name, float volume = 1.0f);
    ~FlxSoundGroup();

    bool add(FlxSound* sound);
    bool remove(FlxSound* sound);
    void pause();
    void resume();
    float getVolume() const;
    float get_volume() const;
    void set_volume(float volume);
    bool get_muted() const;
    void set_muted(bool value);
    std::string name;
    std::vector<FlxSound*> sounds;

private:
    float volume;
    bool muted;
};
}