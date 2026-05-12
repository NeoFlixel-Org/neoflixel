#pragma once
#include <algorithm>
#include <cmath>
#include <string>
#include "FlxPoint.h"

namespace flixel {
namespace math {

class FlxRect {
public:
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;

    FlxRect(float x = 0, float y = 0, float width = 0, float height = 0)
        : x(x), y(y), width(width), height(height) {}
    
    FlxRect(const FlxRect& other)
        : x(other.x), y(other.y), width(other.width), height(other.height) {}

    FlxRect& operator=(const FlxRect& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
            width = other.width;
            height = other.height;
        }
        return *this;
    }

    FlxRect& set(float x = 0, float y = 0, float width = 0, float height = 0) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        return *this;
    }

    FlxRect& setSize(float width, float height) {
        this->width = width;
        this->height = height;
        return *this;
    }

    FlxRect& setPosition(float x, float y) {
        this->x = x;
        this->y = y;
        return *this;
    }

    FlxRect& setBounds(float x1, float y1, float x2, float y2) {
        return set(x1, y1, x2 - x1, y2 - y1);
    }

    FlxRect& setAbs(float x, float y, float width, float height) {
        this->x = width > 0 ? x : x + width;
        this->y = height > 0 ? y : y + height;
        this->width = width > 0 ? width : -width;
        this->height = height > 0 ? height : -height;
        return *this;
    }

    FlxRect& setBoundsAbs(float x1, float y1, float x2, float y2) {
        return setAbs(x1, y1, x2 - x1, y2 - y1);
    }

    float left() const { return x; }
    float right() const { return x + width; }
    float top() const { return y; }
    float bottom() const { return y + height; }

    void setLeft(float value) {
        width -= value - x;
        x = value;
    }

    void setRight(float value) {
        width = value - x;
    }

    void setTop(float value) {
        height -= value - y;
        y = value;
    }

    void setBottom(float value) {
        height = value - y;
    }

    bool isEmpty() const {
        return width == 0 || height == 0;
    }

    bool overlaps(const FlxRect& rect) const {
        return static_cast<float>(rect.right()) > static_cast<float>(left()) &&
               static_cast<float>(rect.left()) < static_cast<float>(right()) &&
               static_cast<float>(rect.bottom()) > static_cast<float>(top()) &&
               static_cast<float>(rect.top()) < static_cast<float>(bottom());
    }

    bool contains(const FlxRect& rect) const {
        return static_cast<float>(rect.left()) >= static_cast<float>(left()) &&
               static_cast<float>(rect.right()) <= static_cast<float>(right()) &&
               static_cast<float>(rect.top()) >= static_cast<float>(top()) &&
               static_cast<float>(rect.bottom()) <= static_cast<float>(bottom());
    }

    bool containsPoint(const FlxPoint& point) const {
        return containsXY(point.x, point.y);
    }

    bool containsXY(float xPos, float yPos) const {
        return static_cast<float>(xPos) >= static_cast<float>(left()) && static_cast<float>(xPos) <= static_cast<float>(right()) &&
               static_cast<float>(yPos) >= static_cast<float>(top()) && static_cast<float>(yPos) <= static_cast<float>(bottom());
    }

    FlxRect& union_(const FlxRect& rect) {
        float minX = std::min(static_cast<float>(x), static_cast<float>(rect.x));
        float minY = std::min(static_cast<float>(y), static_cast<float>(rect.y));
        float maxX = std::max(static_cast<float>(right()), static_cast<float>(rect.right()));
        float maxY = std::max(static_cast<float>(bottom()), static_cast<float>(rect.bottom()));
        return set(minX, minY, maxX - minX, maxY - minY);
    }

    FlxRect& unionWithPoint(const FlxPoint& point) {
        float minX = std::min(static_cast<float>(x), static_cast<float>(point.x));
        float minY = std::min(static_cast<float>(y), static_cast<float>(point.y));
        float maxX = std::max(static_cast<float>(right()), static_cast<float>(point.x));
        float maxY = std::max(static_cast<float>(bottom()), static_cast<float>(point.y));
        return set(minX, minY, maxX - minX, maxY - minY);
    }

    FlxRect& offset(float dx, float dy) {
        x += dx;
        y += dy;
        return *this;
    }

    FlxRect& floor() {
        x = std::floor(x);
        y = std::floor(y);
        width = std::floor(width);
        height = std::floor(height);
        return *this;
    }

    FlxRect& ceil() {
        x = std::ceil(x);
        y = std::ceil(y);
        width = std::ceil(width);
        height = std::ceil(height);
        return *this;
    }

    FlxRect& round() {
        x = std::round(x);
        y = std::round(y);
        width = std::round(width);
        height = std::round(height);
        return *this;
    }

    FlxRect& fromTwoPoints(const FlxPoint& point1, const FlxPoint& point2) {
        float minX = std::min(static_cast<float>(point1.x), static_cast<float>(point2.x));
        float minY = std::min(static_cast<float>(point1.y), static_cast<float>(point2.y));
        float maxX = std::max(static_cast<float>(point1.x), static_cast<float>(point2.x));
        float maxY = std::max(static_cast<float>(point1.y), static_cast<float>(point2.y));
        return set(minX, minY, maxX - minX, maxY - minY);
    }

    FlxPoint getMidpoint(FlxPoint* point) const {
        if (point == nullptr) {
            return FlxPoint(x + 0.5f * width, y + 0.5f * height);
        }
        return point->set(x + 0.5f * width, y + 0.5f * height);
    }

    bool equals(const FlxRect& rect) const {
        return std::abs(x - rect.x) < flixel::FlxPoint::EPSILON &&
               std::abs(y - rect.y) < flixel::FlxPoint::EPSILON &&
               std::abs(width - rect.width) < flixel::FlxPoint::EPSILON &&
               std::abs(height - rect.height) < flixel::FlxPoint::EPSILON;
    }

    std::string toString() const {
        return "FlxRect(x:" + std::to_string(x) + 
               ", y:" + std::to_string(y) + 
               ", w:" + std::to_string(width) + 
               ", h:" + std::to_string(height) + ")";
    }
};

}
}