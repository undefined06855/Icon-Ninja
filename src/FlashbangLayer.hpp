#pragma once
#include <Geode/Geode.hpp>

class FlashbangLayer : public cocos2d::CCLayerColor {
public:
    static FlashbangLayer* create();
    bool init();
    void addSelfToScene();
    void flashAndRemove();
};
