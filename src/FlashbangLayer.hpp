#pragma once
#include <Geode/cocos/include/cocos2d.h>

class FlashbangLayer : public cocos2d::CCLayerColor {
public:
    static FlashbangLayer* create();
    bool init();
    void addSelfToScene();
    void flashAndRemove();
};
