#pragma once
#include <Geode/modify/MenuLayer.hpp>
#include "../NinjaSwipeLayer.hpp"

class $modify(HookedMenuLayer, MenuLayer) {
    static void onModify(auto& self);

    struct Fields {
        std::map<geode::WeakRef<cocos2d::CCNode>, cocos2d::CCPoint> m_movedNodeMovements;
        NinjaSwipeLayer* m_ninjaSwipeLayer;
    };

    bool init();
    void keyDown(cocos2d::enumKeyCodes code, double timestamp);

    void runEnterGameplayAnimations();
	void runExitGameplayAnimations();
};
