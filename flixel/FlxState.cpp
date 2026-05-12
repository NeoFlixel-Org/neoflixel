#include "FlxState.h"
#include "FlxSubState.h"

namespace flixel {

void FlxState::update(float elapsed) {
    if (subState) {
        subState->update(elapsed);
        return;
    }

    for (auto* member : members) {
        if (member) {
            member->update(elapsed);
        }
    }
}

void FlxState::draw() {
    if (subState) {
        subState->draw();
        return;
    }

    for (auto* member : members) {
        if (member) {
            member->draw();
        }
    }
}

void FlxState::destroy() {
    if (subState) {
        subState->destroy();
        delete subState;
        subState = nullptr;
    }

    for (auto* member : members) {
        if (member) {
            member->destroy();
            delete member;
        }
    }
    members.clear();
}

void FlxState::add(FlxBasic* object) {
    if (object) {
        members.push_back(object);
    }
}

void FlxState::remove(FlxBasic* object, bool destroy) {
    if (!object) return;

    auto it = std::find(members.begin(), members.end(), object);
    if (it != members.end()) {
        if (destroy) {
            (*it)->destroy();
            delete *it;
        }
        members.erase(it);
    }
}

void FlxState::clear(bool destroy) {
    if (destroy) {
        for (auto* member : members) {
            if (member) {
                member->destroy();
                delete member;
            }
        }
    }
    members.clear();
}

void FlxState::openSubState(FlxSubState* substate) {
    if (!substate) return;

    if (subState) {
        closeSubState();
    }

    subState = substate;
    subState->_parentState = this;
    subState->create();

    if (subState->openCallback) {
        subState->openCallback();
    }
}

void FlxState::closeSubState() {
    if (!subState) return;

    if (subState->closeCallback) {
        subState->closeCallback();
    }

    subState->destroy();
    delete subState;
    subState = nullptr;
}
}