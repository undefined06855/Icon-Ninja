#include "MenuLayer.hpp"
#include "MenuGameLayer.hpp"
#include "NinjaSwipeLayer.hpp"
#include "utils/random.hpp"

bool HookedMenuLayer::init() {
    if (!MenuLayer::init()) return false;

    // gd lunar compatibility
    if (auto title = geode::cast::typeinfo_cast<cocos2d::CCNodeRGBA*>(getChildByID("main-title"))) {
        title->setCascadeOpacityEnabled(true);
    }

    return true;
}

void HookedMenuLayer::keyDown(cocos2d::enumKeyCodes code) {
    auto mgl = static_cast<HookedMenuGameLayer*>(m_menuGameLayer);
    auto nsl = mgl->m_fields->ninjaSwipeLayer;
    if (nsl->m_state == State::Gameplay) {
        if (code == cocos2d::enumKeyCodes::KEY_Escape) nsl->exitGameplay(nullptr);
        // dont call orig
        return;
    }

    MenuLayer::keyDown(code);
}

#define FADE_OUT_ACTION(movement) cocos2d::CCSpawn::createWithTwoActions(cocos2d::CCEaseBackIn::create(cocos2d::CCMoveBy::create(.9f, movement)), cocos2d::CCFadeOut::create(.9f))
#define FADE_IN_ACTION(movement) cocos2d::CCSpawn::createWithTwoActions(cocos2d::CCEaseBackOut::create(cocos2d::CCMoveBy::create(.9f, -movement)), cocos2d::CCFadeIn::create(.9f))

void HookedMenuLayer::runEnterGameplayAnimations() {
    auto dir = cocos2d::CCDirector::sharedDirector();
    m_fields->movedNodeMovements.clear();

    // dw about this code
    // rewrite it if you wish
    // gets all children and if they're not this figures out where they should
    // move to and stores it in (std::map)m_fields->movedNodeMovements
    for (auto& child : geode::cocos::CCArrayExt<CCNode*>(getChildren())) {
        if (child == m_menuGameLayer) continue;
        
        // get side + move dist
        cocos2d::CCPoint movement;

        if (child->getPositionX() == dir->getScreenRight()/2) {
            // middle
            if (child->getPositionY() < dir->getScreenTop()/2) {
                // bottom middle
                movement = cocos2d::CCPoint{0.f, -child->getPositionY() - child->getContentHeight()};
            } else {
                // top middle (default if in centre)
                movement = cocos2d::CCPoint{0.f, dir->getScreenTop() - child->getPositionY() + child->getContentHeight()};
            }
        } else if (child->getPositionX() < dir->getScreenRight()/2) {
            // left
            movement = cocos2d::CCPoint{-child->getPositionX() - child->getContentWidth(), 0.f};
        } else {
            // right
            movement = cocos2d::CCPoint{(dir->getScreenRight() - child->getPositionX()) + child->getContentWidth(), 0.f};
        }

        child->runAction(FADE_OUT_ACTION(movement));
        m_fields->movedNodeMovements[child] = movement;
    }
}

void HookedMenuLayer::runExitGameplayAnimations() {
    for (auto& pair : m_fields->movedNodeMovements) {
        auto& child = pair.first;
        auto movement = pair.second;
        child->runAction(FADE_IN_ACTION(movement));
    }
}
