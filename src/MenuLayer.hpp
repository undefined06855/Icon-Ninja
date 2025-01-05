#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "NinjaSwipeLayer.hpp"

class $modify(HookedMenuLayer, MenuLayer) {
    static void onModify(auto& self);
    
    struct Fields {
        std::map<cocos2d::CCNode*, cocos2d::CCPoint> movedNodeMovements;
        NinjaSwipeLayer* ninjaSwipeLayer;
    };

    bool init();
    void keyDown(cocos2d::enumKeyCodes code);

    void runEnterGameplayAnimations();
	void runExitGameplayAnimations();
};
