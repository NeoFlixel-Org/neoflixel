#pragma once
#include <string>
#include <vector>
#include <map>
#include <SDL.h>
#include <tinyxml2.h>

namespace flixel {
namespace graphics {
namespace frames {

struct AtlasFrame {
    std::string name;
    SDL_Rect rect;
    SDL_Rect sourceSize;
    int index;
};

class FlxAtlasFrames {
public:
    SDL_Texture* texture = nullptr;
    std::vector<AtlasFrame> frames;
    std::map<std::string, std::vector<int>> prefixToIndices;

    ~FlxAtlasFrames() {
    }

    static FlxAtlasFrames* fromSparrow(const std::string& imagePath, const std::string& xmlText);

    int getFrameIndexByName(const std::string& name) const;
    std::vector<int> getFramesByPrefix(const std::string& prefix) const;
};

}
}
}