#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

struct $modify(HookedMenuLayer, MenuLayer) {
    void keyDown(cocos2d::enumKeyCodes code);
};
