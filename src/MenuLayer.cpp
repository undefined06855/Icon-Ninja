#include "MenuLayer.hpp"
#include "MenuGameLayer.hpp"

void HookedMenuLayer::keyDown(cocos2d::enumKeyCodes code) {
    if (static_cast<HookedMenuGameLayer*>(m_menuGameLayer)->m_fields->isInGameplay) {
        // block keybinds when in gameplay
        return;
    }

    MenuLayer::keyDown(code);
}
