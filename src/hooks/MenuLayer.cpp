#include "MenuLayer.hpp"
#include "../utils/log.hpp"
#include "../shaders.hpp"

void HookedMenuLayer::onModify(auto& self) {
    ninja::log::info("hook prio time");
    if (!self.setHookPriority("MenuLayer::init", geode::Priority::VeryLatePost)) {
        ninja::log::warn("failed to set hook prio??? compats may be broken");
    }
}

bool HookedMenuLayer::init() {
    if (!MenuLayer::init()) return false;

    // gd lunar compatibility
    if (auto title = geode::cast::typeinfo_cast<cocos2d::CCNodeRGBA*>(getChildByID("main-title"))) {
        title->setCascadeOpacityEnabled(true);
    }

    // day and night system compatibility
    if (auto dnsEventNode = getChildByID("ninsam.day_and_night_system/Events")) {
        ninja::log::info("day and night system momento"); 
        dnsEventNode->setZOrder(-2);
    }

    // gd compatibility (real)
    // ensure cascade opacity is enabled for children of buttons and such (really just circlebuttonsprite)
    std::array<std::string, 5> menuNames = {
        "bottom-menu",
        "right-side-menu",
        "top-right-menu",
        "side-menu",
        "close-menu"
    };

    for (auto& name : menuNames) {
        auto menu = getChildByID(name);
        if (!menu) continue;

        for (auto& child : geode::cocos::CCArrayExt<cocos2d::CCNode*>(menu->getChildren())) {
            for (auto& innerChild : geode::cocos::CCArrayExt<cocos2d::CCNode*>(child->getChildren())) {
                auto castInnerChild = geode::cast::typeinfo_cast<cocos2d::CCNodeRGBA*>(innerChild);
                if (!castInnerChild) continue;
                castInnerChild->setCascadeOpacityEnabled(true);
            }
        }
    }

    if (g_swipeShaderFailedToLoad) {
        auto pop = FLAlertLayer::create(
            "Icon Ninja",
            "The <co>shader</c> for the <cy>swipe trail</c> did <cr>not</c> "
            "compile properly!"
            "\n\n"
            "Please report this as a <cr>bug</c>, and send your "
            "<co>log files</c> when reporting!",
            "ok"
        );
        pop->m_scene = this;
        pop->show();
    }

    // add our very own brand spanking new shiny icon ninja custom layer!!!!!!!!
    m_fields->m_ninjaSwipeLayer = NinjaSwipeLayer::create();
    addChild(m_fields->m_ninjaSwipeLayer, -1);
    
    m_menuGameLayer->setZOrder(-5); // get back there

    return true;
}

void HookedMenuLayer::keyDown(cocos2d::enumKeyCodes code) {
    auto ninja = m_fields->m_ninjaSwipeLayer;

    if (ninja->m_state == State::Gameplay) {
        if (code == cocos2d::enumKeyCodes::KEY_Escape) ninja->exitGameplay(nullptr);
        // dont call orig
        return;
    }

    MenuLayer::keyDown(code);
}

#define FADE_OUT_ACTION(movement) cocos2d::CCSpawn::createWithTwoActions(cocos2d::CCEaseBackIn::create(cocos2d::CCMoveBy::create(.9f, movement)), cocos2d::CCFadeOut::create(.9f))
#define FADE_IN_ACTION(movement) cocos2d::CCSpawn::createWithTwoActions(cocos2d::CCEaseBackOut::create(cocos2d::CCMoveBy::create(.9f, -movement)), cocos2d::CCFadeIn::create(.9f))

void HookedMenuLayer::runEnterGameplayAnimations() {
    auto dir = cocos2d::CCDirector::sharedDirector();
    m_fields->m_movedNodeMovements.clear();

    // gets all children and if they should be moved figures out where they
    // should move to and stores it in (std::map)m_fields->movedNodeMovements
    for (auto& child : geode::cocos::CCArrayExt<CCNode*>(getChildren())) {
        if (child->getZOrder() <= m_fields->m_ninjaSwipeLayer->getZOrder()) continue;
        
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
        m_fields->m_movedNodeMovements[child] = movement;
    }
}

void HookedMenuLayer::runExitGameplayAnimations() {
    for (auto& [_node, movement] : m_fields->m_movedNodeMovements) {
        if (auto node = _node.lock()) {
            node->runAction(FADE_IN_ACTION(movement));
        }
    }
}

#undef FADE_OUT_ACTION
#undef FADE_IN_ACTION
