#pragma once
#include <string>
#include <algorithm>

namespace flixel {
namespace util {

enum class FlxAxes : int {
    NONE = 0x00,
    X    = 0x01,
    Y    = 0x10,
    XY   = 0x11
};

class FlxAxesHelper {
public:
    static bool hasX(FlxAxes axes) {
        return static_cast<int>(axes) & static_cast<int>(FlxAxes::X);
    }
    
    static bool hasY(FlxAxes axes) {
        return static_cast<int>(axes) & static_cast<int>(FlxAxes::Y);
    }
    
    static FlxAxes fromBools(bool x, bool y) {
        int value = (x ? static_cast<int>(FlxAxes::X) : 0) | 
                   (y ? static_cast<int>(FlxAxes::Y) : 0);
        return static_cast<FlxAxes>(value);
    }
    
    static FlxAxes fromString(const std::string& axes) {
        std::string lower = axes;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "x") return FlxAxes::X;
        if (lower == "y") return FlxAxes::Y;
        if (lower == "xy" || lower == "yx" || lower == "both") return FlxAxes::XY;
        if (lower == "none" || lower.empty()) return FlxAxes::NONE;
        
        throw std::runtime_error("Invalid axes value: " + axes);
    }
    
    static std::string toString(FlxAxes axes) {
        switch (axes) {
            case FlxAxes::X: return "x";
            case FlxAxes::Y: return "y";
            case FlxAxes::XY: return "xy";
            case FlxAxes::NONE: return "none";
            default: return "unknown";
        }
    }
};

}
}