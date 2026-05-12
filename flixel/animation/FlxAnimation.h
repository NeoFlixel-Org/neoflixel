#pragma once
#include <string>
#include <vector>

namespace flixel {
namespace animation {

struct FlxAnimation {
    std::string name;
    std::vector<int> frames;
    int frameRate;
    bool looped;
};

}
}