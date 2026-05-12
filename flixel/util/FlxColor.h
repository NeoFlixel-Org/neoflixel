#pragma once
#include <cstdint>
#include <string>
#include <algorithm>
#include <cmath>

namespace flixel {
namespace util {

class FlxColor {
public:
    using uint32 = uint32_t;
    uint32 value;

    static constexpr uint32 TRANSPARENT = 0x00000000;
    static constexpr uint32 WHITE       = 0xFFFFFFFF;
    static constexpr uint32 GRAY        = 0xFF808080;
    static constexpr uint32 BLACK       = 0xFF000000;
    static constexpr uint32 GREEN       = 0xFF008000;
    static constexpr uint32 LIME        = 0xFF00FF00;
    static constexpr uint32 YELLOW      = 0xFFFFFF00;
    static constexpr uint32 ORANGE      = 0xFFFFA500;
    static constexpr uint32 RED         = 0xFFFF0000;
    static constexpr uint32 PURPLE      = 0xFF800080;
    static constexpr uint32 BLUE        = 0xFF0000FF;
    static constexpr uint32 BROWN       = 0xFF8B4513;
    static constexpr uint32 PINK        = 0xFFFFC0CB;
    static constexpr uint32 MAGENTA     = 0xFFFF00FF;
    static constexpr uint32 CYAN        = 0xFF00FFFF;

    FlxColor(uint32 argb = WHITE) : value(argb) {}
    FlxColor(int r, int g, int b, int a = 255) {
        setRGBA(r, g, b, a);
    }

    int alpha() const { return (value >> 24) & 0xFF; }
    int red()   const { return (value >> 16) & 0xFF; }
    int green() const { return (value >> 8)  & 0xFF; }
    int blue()  const { return value & 0xFF; }

    float alphaF() const { return alpha() / 255.0f; }
    float redF()   const { return red() / 255.0f; }
    float greenF() const { return green() / 255.0f; }
    float blueF()  const { return blue() / 255.0f; }

    void setAlpha(int a) { value = (value & 0x00FFFFFF) | ((a & 0xFF) << 24); }
    void setRed(int r)   { value = (value & 0xFF00FFFF) | ((r & 0xFF) << 16); }
    void setGreen(int g) { value = (value & 0xFFFF00FF) | ((g & 0xFF) << 8); }
    void setBlue(int b)  { value = (value & 0xFFFFFF00) | (b & 0xFF); }

    void setRGBA(int r, int g, int b, int a = 255) {
        value = ((a & 0xFF) << 24) |
                ((r & 0xFF) << 16) |
                ((g & 0xFF) << 8)  |
                (b & 0xFF);
    }

    void setRGBAf(float r, float g, float b, float a = 1.0f) {
        setRGBA(
            static_cast<int>(r * 255),
            static_cast<int>(g * 255),
            static_cast<int>(b * 255),
            static_cast<int>(a * 255)
        );
    }

    static FlxColor fromRGB(int r, int g, int b, int a = 255) {
        return FlxColor(r, g, b, a);
    }
    static FlxColor fromRGBf(float r, float g, float b, float a = 1.0f) {
        return FlxColor(
            static_cast<int>(r * 255),
            static_cast<int>(g * 255),
            static_cast<int>(b * 255),
            static_cast<int>(a * 255)
        );
    }
    static FlxColor fromARGB(uint32 argb) {
        return FlxColor(argb);
    }

    bool operator==(const FlxColor& other) const { return value == other.value; }
    bool operator!=(const FlxColor& other) const { return value != other.value; }

    std::string toHexString(bool withAlpha = true) const {
        char buf[11];
        if (withAlpha)
            snprintf(buf, sizeof(buf), "0x%02X%02X%02X%02X", alpha(), red(), green(), blue());
        else
            snprintf(buf, sizeof(buf), "0x%02X%02X%02X", red(), green(), blue());
        return std::string(buf);
    }

    static FlxColor interpolate(const FlxColor& c1, const FlxColor& c2, float t) {
        int r = static_cast<int>(c1.red()   + (c2.red()   - c1.red())   * t);
        int g = static_cast<int>(c1.green() + (c2.green() - c1.green()) * t);
        int b = static_cast<int>(c1.blue()  + (c2.blue()  - c1.blue())  * t);
        int a = static_cast<int>(c1.alpha() + (c2.alpha() - c1.alpha()) * t);
        return FlxColor(r, g, b, a);
    }

    FlxColor inverted() const {
        return FlxColor(255 - red(), 255 - green(), 255 - blue(), alpha());
    }
};

}
} 