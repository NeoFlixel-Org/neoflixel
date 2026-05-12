#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#include <cmath>
#include <string>

namespace flixel {

class FlxPoint {
public:
    static constexpr float EPSILON = 0.0000001f;
    static constexpr float EPSILON_SQUARED = EPSILON * EPSILON;

    float x = 0;
    float y = 0;

    FlxPoint(float x = 0, float y = 0) : x(x), y(y) {}
    FlxPoint(const FlxPoint& other) : x(other.x), y(other.y) {}

    FlxPoint& operator=(const FlxPoint& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }

    FlxPoint& set(float x = 0, float y = 0) {
        this->x = x;
        this->y = y;
        return *this;
    }

    FlxPoint& add(float x = 0, float y = 0) {
        this->x += x;
        this->y += y;
        return *this;
    }

    FlxPoint& addPoint(const FlxPoint& point) {
        return add(point.x, point.y);
    }

    FlxPoint& subtract(float x = 0, float y = 0) {
        this->x -= x;
        this->y -= y;
        return *this;
    }

    FlxPoint& subtractPoint(const FlxPoint& point) {
        return subtract(point.x, point.y);
    }

    FlxPoint& scale(float x, float y) {
        this->x *= x;
        this->y *= y;
        return *this;
    }

    FlxPoint& scale(float amount) {
        return scale(amount, amount);
    }

    FlxPoint operator+(const FlxPoint& other) const {
        return FlxPoint(x + other.x, y + other.y);
    }

    FlxPoint operator-(const FlxPoint& other) const {
        return FlxPoint(x - other.x, y - other.y);
    }

    FlxPoint operator*(float scalar) const {
        return FlxPoint(x * scalar, y * scalar);
    }

    FlxPoint& operator+=(const FlxPoint& other) {
        return addPoint(other);
    }

    FlxPoint& operator-=(const FlxPoint& other) {
        return subtractPoint(other);
    }

    FlxPoint& operator*=(float scalar) {
        return scale(scalar);
    }

    float length() const {
        return std::sqrt(lengthSquared());
    }

    float lengthSquared() const {
        return x * x + y * y;
    }

    float distanceTo(const FlxPoint& point) const {
        float dx = x - point.x;
        float dy = y - point.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    float distanceSquaredTo(const FlxPoint& point) const {
        float dx = x - point.x;
        float dy = y - point.y;
        return dx * dx + dy * dy;
    }

    float dotProduct(const FlxPoint& point) const {
        return x * point.x + y * point.y;
    }

    float crossProductLength(const FlxPoint& point) const {
        return x * point.y - y * point.x;
    }

    bool isZero() const {
        return std::abs(x) < EPSILON && std::abs(y) < EPSILON;
    }

    FlxPoint& zero() {
        x = y = 0;
        return *this;
    }

    FlxPoint& normalize() {
        if (!isZero()) {
            float len = length();
            x /= len;
            y /= len;
        }
        return *this;
    }

    bool isNormalized() const {
        return std::abs(lengthSquared() - 1) < EPSILON_SQUARED;
    }

    float radians() const {
        return std::atan2(y, x);
    }

    float degrees() const {
        return radians() * 180.0f / M_PI;
    }

    FlxPoint& rotateByRadians(float rads) {
        float cos = std::cos(rads);
        float sin = std::sin(rads);
        float tempX = x;
        x = tempX * cos - y * sin;
        y = tempX * sin + y * cos;
        return *this;
    }

    FlxPoint& rotateByDegrees(float degs) {
        return rotateByRadians(degs * M_PI / 180.0f);
    }

    bool equals(const FlxPoint& point) const {
        return std::abs(x - point.x) < EPSILON && std::abs(y - point.y) < EPSILON;
    }

    std::string toString() const {
        return "FlxPoint(x:" + std::to_string(x) + ", y:" + std::to_string(y) + ")";
    }
};

inline FlxPoint operator*(float scalar, const FlxPoint& point) {
    return point * scalar;
}

}