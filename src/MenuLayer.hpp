#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

struct $modify(HookedMenuLayer, MenuLayer) {
    struct Fields {
        std::map<cocos2d::CCNode*, cocos2d::CCPoint> movedNodeMovements;
    };

    bool init();
    void keyDown(cocos2d::enumKeyCodes code);

    void runEnterGameplayAnimations();
	void runExitGameplayAnimations();
};
