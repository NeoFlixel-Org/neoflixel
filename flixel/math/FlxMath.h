#pragma once

#include <cmath>
#include <algorithm>
#include <string>
#include <cfloat>
#include <limits>
#include "../FlxBasic.h"
#include "FlxPoint.h"
#include "FlxRect.h"
#include "../FlxSprite.h"

namespace flixel {

namespace math {

constexpr float MIN_VALUE_FLOAT = std::numeric_limits<float>::min();
constexpr float MAX_VALUE_FLOAT = std::numeric_limits<float>::max();
constexpr int MIN_VALUE_INT = std::numeric_limits<int>::min();
constexpr int MAX_VALUE_INT = std::numeric_limits<int>::max();
constexpr float SQUARE_ROOT_OF_TWO = 1.41421356237f;
constexpr float PI = 3.14159265358979323846f;
constexpr float EPSILON = 0.0000001f;

inline float roundDecimal(float value, int precision) {
    float mult = 1.0f;
    for (int i = 0; i < precision; i++) {
        mult *= 10.0f;
    }
    return std::round(value * mult) / mult;
}

inline float bound(float value, float min = std::numeric_limits<float>::lowest(), float max = std::numeric_limits<float>::max()) {
    float lowerBound = (value < min) ? min : value;
    return (lowerBound > max) ? max : lowerBound;
}

inline float lerp(float a, float b, float ratio) {
    return a + ratio * (b - a);
}

inline float getElapsedLerp(float lerp, float elapsed) {
    return 1.0f - std::pow(1.0f - lerp, elapsed * 60.0f);
}

inline bool inBounds(float value, float min = std::numeric_limits<float>::lowest(), float max = std::numeric_limits<float>::max()) {
    return (value >= min) && (value <= max);
}

inline bool isOdd(float n) {
    return (static_cast<int>(n) & 1) != 0;
}

inline bool isEven(float n) {
    return (static_cast<int>(n) & 1) == 0;
}

inline int numericComparison(float a, float b) {
    if (b > a) return -1;
    if (a > b) return 1;
    return 0;
}

inline bool pointInCoordinates(float pointX, float pointY, float rectX, float rectY, float rectWidth, float rectHeight) {
    return (pointX >= rectX && pointX <= (rectX + rectWidth)) &&
           (pointY >= rectY && pointY <= (rectY + rectHeight));
}

inline bool pointInFlxRect(float pointX, float pointY, const FlxRect& rect) {
    return rect.containsXY(pointX, pointY);
}

inline bool pointInRectangle(float pointX, float pointY, const SDL_Rect& rect) {
    return pointX >= rect.x && pointX <= rect.x + rect.w &&
           pointY >= rect.y && pointY <= rect.y + rect.h;
}

inline int maxAdd(int value, int amount, int max, int min = 0) {
    value += amount;
    if (value > max) value = max;
    else if (value <= min) value = min;
    return value;
}

inline int wrap(int value, int min, int max) {
    int range = max - min + 1;
    if (value < min) {
        value += range * static_cast<int>((min - value) / range + 1);
    }
    return min + (value - min) % range;
}

inline float remapToRange(float value, float start1, float stop1, float start2, float stop2) {
    return start2 + (value - start1) * ((stop2 - start2) / (stop1 - start1));
}

inline float dotProduct(float ax, float ay, float bx, float by) {
    return ax * bx + ay * by;
}

inline float vectorLength(float dx, float dy) {
    return std::sqrt(dx * dx + dy * dy);
}

inline int distanceBetween(const FlxSprite& spriteA, const FlxSprite& spriteB) {
    float dx = (spriteA.x + spriteA.originX) - (spriteB.x + spriteB.originX);
    float dy = (spriteA.y + spriteA.originY) - (spriteB.y + spriteB.originY);
    return static_cast<int>(vectorLength(dx, dy));
}

inline bool isDistanceWithin(const FlxSprite& spriteA, const FlxSprite& spriteB, float distance, bool includeEqual = false) {
    float dx = (spriteA.x + spriteA.originX) - (spriteB.x + spriteB.originX);
    float dy = (spriteA.y + spriteA.originY) - (spriteB.y + spriteB.originY);
    float distSquared = dx * dx + dy * dy;
    return includeEqual ? distSquared <= distance * distance : distSquared < distance * distance;
}

inline int getDecimals(float n) {
    std::string str = std::to_string(n);
    size_t pos = str.find('.');
    return (pos != std::string::npos) ? str.length() - pos - 1 : 0;
}

inline bool equal(float valueA, float valueB, float diff = EPSILON) {
    return std::abs(valueA - valueB) <= diff;
}

inline int signOf(float n) {
    return (n < 0) ? -1 : 1;
}


inline bool sameSign(float a, float b) {
    return signOf(a) == signOf(b);
}

inline float fastSin(float n) {
    n *= 0.3183098862f;

    if (n > 1) {
        n -= (static_cast<int>(std::ceil(n)) >> 1) << 1;
    }
    else if (n < -1) {
        n += (static_cast<int>(std::ceil(-n)) >> 1) << 1;
    }

    if (n > 0) {
        return n * (3.1f + n * (0.5f + n * (-7.2f + n * 3.6f)));
    }
    else {
        return n * (3.1f - n * (0.5f + n * (7.2f + n * 3.6f)));
    }
}

inline float fastCos(float n) {
    return fastSin(n + 1.570796327f);
}

inline float sinh(float n) {
    return (std::exp(n) - std::exp(-n)) / 2.0f;
}

inline int maxInt(int a, int b) {
    return (a > b) ? a : b;
}

inline int minInt(int a, int b) {
    return (a > b) ? b : a;
}

inline int absInt(int n) {
    return (n > 0) ? n : -n;
}

inline float mod(float a, float b) {
    b = std::abs(b);
    return a - b * std::floor(a / b);
}

inline float abs(float value) { return std::abs(value); }
inline float min(float a, float b) { return std::min(a, b); }
inline float max(float a, float b) { return std::max(a, b); }
inline float clamp(float value, float min, float max) { return std::clamp(value, min, max); }
inline float round(float value) { return std::round(value); }
inline float floor(float value) { return std::floor(value); }
inline float ceil(float value) { return std::ceil(value); }
inline float sin(float angle) { return std::sin(angle); }
inline float cos(float angle) { return std::cos(angle); }
inline float tan(float angle) { return std::tan(angle); }
inline float atan2(float y, float x) { return std::atan2(y, x); }
inline float sqrt(float value) { return std::sqrt(value); }
inline float pow(float base, float exponent) { return std::pow(base, exponent); }

inline bool overlap(FlxSprite* sprite1, FlxSprite* sprite2) {
    if (sprite1 == nullptr || sprite2 == nullptr) return false;

    float dx = sprite1->x - sprite2->x;
    float dy = sprite1->y - sprite2->y;
    float distance = std::sqrt(dx * dx + dy * dy);

    float minDist = sprite1->originX + sprite2->originX;
    return distance < minDist;
}

inline bool overlapAt(FlxSprite* sprite1, float x1, float y1, FlxSprite* sprite2, float x2, float y2) {
    if (sprite1 == nullptr || sprite2 == nullptr) return false;

    float dx = x1 - x2;
    float dy = y1 - y2;
    float distance = std::sqrt(dx * dx + dy * dy);

    float minDist = sprite1->originX + sprite2->originX;
    return distance < minDist;
}

inline bool overlapPoint(FlxSprite* sprite, float x, float y) {
    if (sprite == nullptr) return false;

    float dx = sprite->x - x;
    float dy = sprite->y - y;
    float distance = std::sqrt(dx * dx + dy * dy);

    return distance < sprite->originX;
}
}
}