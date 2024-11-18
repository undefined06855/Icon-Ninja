#pragma once
#include "Swipe.hpp"

class NinjaSwipeLayer : public cocos2d::CCLayer {
public:
    static NinjaSwipeLayer* create();
    bool init() override;

    bool m_isFingerDown = false;
    Swipe* m_swipe;
    cocos2d::CCPoint m_lastSwipePoint;

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

    void checkSwipeIntersection(const cocos2d::CCPoint& from, const cocos2d::CCPoint& to);
    bool lineIntersectsRect(const cocos2d::CCRect& rect, const cocos2d::CCPoint& p1, const cocos2d::CCPoint& p2);
};
