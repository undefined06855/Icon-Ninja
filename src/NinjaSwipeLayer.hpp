#pragma once
#include <Geode/Geode.hpp>

class NinjaSwipeLayer : public cocos2d::CCLayer {
public:
    static NinjaSwipeLayer* create();
    bool init() override;

    cocos2d::CCPoint m_lastTouchPoint;
    HardStreak* m_hardStreak;

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

    void checkSwipeIntersection(cocos2d::CCPoint& point, cocos2d::CCPoint& last);
};
