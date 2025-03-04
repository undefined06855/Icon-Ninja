#pragma once
#include <Geode/cocos/include/cocos2d.h>

class CCBrighten : public cocos2d::CCActionInterval {
public:
    static CCBrighten* create(float duration);

    void update(float time);
};
