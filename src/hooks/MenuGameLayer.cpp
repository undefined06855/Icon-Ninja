#include "MenuGameLayer.hpp"
#include <Geode/binding/PlayerObject.hpp>

bool HookedMenuGameLayer::init() {
    if (!MenuGameLayer::init()) return false;

    // remove touch from this
    setTouchEnabled(false);
    cocos2d::CCTouchDispatcher::get()->removeDelegate(this);

    // remove other shit
    m_groundLayer->setVisible(false);

    return true;
}

void HookedMenuGameLayer::update(float dt) {
    MenuGameLayer::update(dt);
    m_playerObject->setPosition({-999.f, -999.f});
}
