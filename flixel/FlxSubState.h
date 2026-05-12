#pragma once

#include "FlxState.h"
#include "util/FlxColor.h"
#include <functional>

namespace flixel {

class FlxCamera;

class FlxSubState : public FlxState {
public:
    using OpenCallback = std::function<void()>;
    using CloseCallback = std::function<void()>;

    explicit FlxSubState(const util::FlxColor& bgColor = util::FlxColor::TRANSPARENT);
    virtual ~FlxSubState();

    OpenCallback openCallback;
    CloseCallback closeCallback;

    virtual void draw() override;
    virtual void destroy() override;
    void close();

    const util::FlxColor& getBgColor() const { return _bgColor; }
    void setBgColor(const util::FlxColor& value);

protected:
    FlxState* _parentState = nullptr;
    util::FlxColor _bgColor{util::FlxColor::TRANSPARENT};
    bool _created = false;

    friend class FlxState;
};
}