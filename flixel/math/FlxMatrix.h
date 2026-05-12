#pragma once
#include <cmath>

namespace flixel {
namespace math {

class FlxMatrix {
public:
    float a, b, c, d, tx, ty;

    FlxMatrix(float a = 1, float b = 0, float c = 0, float d = 1, float tx = 0, float ty = 0)
        : a(a), b(b), c(c), d(d), tx(tx), ty(ty) {}

    void setTo(float a, float b, float c, float d, float tx, float ty) {
        this->a = a; this->b = b; this->c = c; this->d = d; this->tx = tx; this->ty = ty;
    }

    FlxMatrix& identity() {
        setTo(1, 0, 0, 1, 0, 0);
        return *this;
    }

    FlxMatrix& rotate(float radians) {
        float cos = std::cos(radians);
        float sin = std::sin(radians);
        float a1 = a * cos - b * sin;
        b = a * sin + b * cos;
        a = a1;
        float c1 = c * cos - d * sin;
        d = c * sin + d * cos;
        c = c1;
        float tx1 = tx * cos - ty * sin;
        ty = tx * sin + ty * cos;
        tx = tx1;
        return *this;
    }

    FlxMatrix& rotateWithTrig(float cos, float sin) {
        float a1 = a * cos - b * sin;
        b = a * sin + b * cos;
        a = a1;
        float c1 = c * cos - d * sin;
        d = c * sin + d * cos;
        c = c1;
        float tx1 = tx * cos - ty * sin;
        ty = tx * sin + ty * cos;
        tx = tx1;
        return *this;
    }

    FlxMatrix& rotateBy180() {
        setTo(-a, -b, -c, -d, -tx, -ty);
        return *this;
    }

    FlxMatrix& rotateByPositive90() {
        setTo(-b, a, -d, c, -ty, tx);
        return *this;
    }

    FlxMatrix& rotateByNegative90() {
        setTo(b, -a, d, -c, ty, -tx);
        return *this;
    }

    float transformX(float px, float py) const {
        return px * a + py * c + tx;
    }

    float transformY(float px, float py) const {
        return px * b + py * d + ty;
    }
};

}
} 