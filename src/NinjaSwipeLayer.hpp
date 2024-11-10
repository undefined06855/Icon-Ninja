#pragma once
// #include "CCBlade.hpp"

class NinjaSwipeLayer : public cocos2d::CCLayer {
public:
    static NinjaSwipeLayer* create();
    bool init() override;

    // CCBlade* m_blade;
    cocos2d::CCPoint m_lastSwipePoint;

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

    void checkSwipeIntersection(const cocos2d::CCPoint& from, const cocos2d::CCPoint& to);
};
