#pragma once
#include <Geode/cocos/include/cocos2d.h>

class SwipePoint {
public:
    SwipePoint(cocos2d::CCPoint location, float direction);

    cocos2d::CCPoint m_location;
    float m_direction;
    float m_tick = 0.f;
    const float m_tickLength = 0.15f;

    std::pair<cocos2d::CCPoint, cocos2d::CCPoint> calculatePointPositions();
    float getDistanceFromTick();
};
