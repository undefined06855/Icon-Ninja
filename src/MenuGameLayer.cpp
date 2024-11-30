#include "MenuGameLayer.hpp"
#include "NinjaSwipeLayer.hpp"

bool HookedMenuGameLayer::init() {
    if (!MenuGameLayer::init()) return false;

    // add custom layer to detect touch and draw the swipe
    m_fields->ninjaSwipeLayer = NinjaSwipeLayer::create();
    m_fields->ninjaSwipeLayer->setZOrder(999);
    addChild(m_fields->ninjaSwipeLayer);
    // remove touch from this
    setTouchEnabled(false);
    cocos2d::CCTouchDispatcher::get()->removeDelegate(this);

    // remove shit
    m_groundLayer->setVisible(false);

    return true;
}

void HookedMenuGameLayer::update(float dt) {
    MenuGameLayer::update(dt);
    m_playerObject->setPosition({-999.f, -999.f});
}
