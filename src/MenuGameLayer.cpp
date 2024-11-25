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

    // remove shit
    m_groundLayer->setVisible(false);

    return true;
}

void HookedMenuGameLayer::update(float dt) {
    MenuGameLayer::update(dt);
    m_playerObject->setPosition({-999.f, -999.f});
}

void HookedMenuGameLayer::randomisePlayerObject(PlayerObject* player) {
    geode::log::info("before: {}", player->retainCount());
    PlayerObject* old = m_playerObject;
    cocos2d::CCPoint pos = player->getPosition();
    m_playerObject = player;
    resetPlayer();
    player->setPosition(pos);
    m_playerObject = old;
    geode::log::info("after: {}", player->retainCount());
}
