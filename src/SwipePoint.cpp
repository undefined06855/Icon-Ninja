#include "SwipePoint.hpp"

SwipePoint::SwipePoint(cocos2d::CCPoint location, float direction) {
    m_location = location;
    m_direction = direction;
}

std::pair<cocos2d::CCPoint, cocos2d::CCPoint> SwipePoint::calculatePointPositions() {
    float angle1 = fmod(m_direction + 1.5708f, 2.f * M_PI);
    float angle2 = fmod(m_direction - 1.5708f, 2.f * M_PI);

    float dist = getDistanceFromTick();

    cocos2d::CCPoint p1 = m_location + cocos2d::CCPoint{ cos(angle1)*dist, sin(angle1)*dist };
    cocos2d::CCPoint p2 = m_location + cocos2d::CCPoint{ cos(angle2)*dist, sin(angle2)*dist };
    return { p1, p2 };
}

float SwipePoint::getDistanceFromTick() {
    // https://www.desmos.com/calculator/mcb1ys1alb
    if (m_tick < 0.03f) {
        return m_tick * 185.f;
    } else {
        return -m_tick * 46.f + 6.9f;
    }
}
