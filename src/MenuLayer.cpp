#include "MenuLayer.hpp"
#include "MenuGameLayer.hpp"

void HookedMenuLayer::keyDown(cocos2d::enumKeyCodes code) {
    geode::log::info("keydown");
    if (static_cast<HookedMenuGameLayer*>(m_menuGameLayer)->m_fields->isInGameplay) {
        geode::log::info("GET BLOCKED");
        return;
    }

    MenuLayer::keyDown(code);
}
