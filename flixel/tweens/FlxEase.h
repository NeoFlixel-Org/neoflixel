#pragma once

#include <cmath>

namespace flixel {
namespace tweens {

typedef float (*EaseFunction)(float);

class FlxEase {
public:
    static constexpr float PI2 = 3.14159265359f / 2.0f;
    static constexpr float ELASTIC_AMPLITUDE = 1.0f;
    static constexpr float ELASTIC_PERIOD = 0.4f;
    
    static inline float linear(float t) {
        return t;
    }
    
    static inline float quadIn(float t) {
        return t * t;
    }
    
    static inline float quadOut(float t) {
        return -t * (t - 2.0f);
    }
    
    static inline float quadInOut(float t) {
        return t <= 0.5f ? t * t * 2.0f : 1.0f - (--t) * t * 2.0f;
    }
    
    static inline float cubeIn(float t) {
        return t * t * t;
    }
    
    static inline float cubeOut(float t) {
        return 1.0f + (--t) * t * t;
    }
    
    static inline float cubeInOut(float t) {
        return t <= 0.5f ? t * t * t * 4.0f : 1.0f + (--t) * t * t * 4.0f;
    }
    
    static inline float quintIn(float t) {
        return t * t * t * t * t;
    }
    
    static inline float quintOut(float t) {
        return (t = t - 1.0f) * t * t * t * t + 1.0f;
    }
    
    static inline float quintInOut(float t) {
        return ((t *= 2.0f) < 1.0f) ? (t * t * t * t * t) / 2.0f : ((t -= 2.0f) * t * t * t * t + 2.0f) / 2.0f;
    }
    
    static inline float sineIn(float t) {
        return -std::cos(PI2 * t) + 1.0f;
    }
    
    static inline float sineOut(float t) {
        return std::sin(PI2 * t);
    }
    
    static inline float sineInOut(float t) {
        return -std::cos(3.14159265359f * t) / 2.0f + 0.5f;
    }
    
    static inline float bounceIn(float t) {
        t = 1.0f - t;
        if (t < 1.0f / 2.75f)
            return 1.0f - (7.5625f * t * t);
        if (t < 2.0f / 2.75f) {
            t -= 1.5f / 2.75f;
            return 1.0f - (7.5625f * t * t + 0.75f);
        }
        if (t < 2.5f / 2.75f) {
            t -= 2.25f / 2.75f;
            return 1.0f - (7.5625f * t * t + 0.9375f);
        }
        t -= 2.625f / 2.75f;
        return 1.0f - (7.5625f * t * t + 0.984375f);
    }
    
    static inline float bounceOut(float t) {
        if (t < 1.0f / 2.75f)
            return 7.5625f * t * t;
        if (t < 2.0f / 2.75f) {
            t -= 1.5f / 2.75f;
            return 7.5625f * t * t + 0.75f;
        }
        if (t < 2.5f / 2.75f) {
            t -= 2.25f / 2.75f;
            return 7.5625f * t * t + 0.9375f;
        }
        t -= 2.625f / 2.75f;
        return 7.5625f * t * t + 0.984375f;
    }
    
    static inline float bounceInOut(float t) {
        if (t < 0.5f)
            return bounceIn(t * 2.0f) * 0.5f;
        return bounceOut(t * 2.0f - 1.0f) * 0.5f + 0.5f;
    }
    
    static inline float circIn(float t) {
        return -(std::sqrt(1.0f - t * t) - 1.0f);
    }
    
    static inline float circOut(float t) {
        return std::sqrt(1.0f - (t - 1.0f) * (t - 1.0f));
    }
    
    static inline float circInOut(float t) {
        return t <= 0.5f ? (std::sqrt(1.0f - t * t * 4.0f) - 1.0f) / -2.0f : (std::sqrt(1.0f - (t * 2.0f - 2.0f) * (t * 2.0f - 2.0f)) + 1.0f) / 2.0f;
    }
    
    static inline float expoIn(float t) {
        return std::pow(2.0f, 10.0f * (t - 1.0f));
    }
    
    static inline float expoOut(float t) {
        return -std::pow(2.0f, -10.0f * t) + 1.0f;
    }
    
    static inline float expoInOut(float t) {
        return t < 0.5f ? std::pow(2.0f, 10.0f * (t * 2.0f - 1.0f)) / 2.0f : (-std::pow(2.0f, -10.0f * (t * 2.0f - 1.0f)) + 2.0f) / 2.0f;
    }
    
    static inline float backIn(float t) {
        return t * t * (2.70158f * t - 1.70158f);
    }
    
    static inline float backOut(float t) {
        return 1.0f - (--t) * t * (-2.70158f * t - 1.70158f);
    }
    
    static inline float backInOut(float t) {
        t *= 2.0f;
        if (t < 1.0f) return t * t * (2.70158f * t - 1.70158f) / 2.0f;
        t--;
        return (1.0f - (--t) * t * (-2.70158f * t - 1.70158f)) / 2.0f + 0.5f;
    }
    
    static inline float elasticIn(float t) {
        return -(ELASTIC_AMPLITUDE * std::pow(2.0f, 10.0f * (t -= 1.0f)) * std::sin((t - (ELASTIC_PERIOD / (2.0f * 3.14159265359f) * std::asin(1.0f / ELASTIC_AMPLITUDE))) * (2.0f * 3.14159265359f) / ELASTIC_PERIOD));
    }
    
    static inline float elasticOut(float t) {
        return (ELASTIC_AMPLITUDE * std::pow(2.0f, -10.0f * t) * std::sin((t - (ELASTIC_PERIOD / (2.0f * 3.14159265359f) * std::asin(1.0f / ELASTIC_AMPLITUDE))) * (2.0f * 3.14159265359f) / ELASTIC_PERIOD) + 1.0f);
    }
    
    static inline float elasticInOut(float t) {
        if (t < 0.5f) {
            return elasticIn(t * 2.0f) / 2.0f;
        }
        return elasticOut(t * 2.0f - 1.0f) / 2.0f + 0.5f;
    }
};
}
}