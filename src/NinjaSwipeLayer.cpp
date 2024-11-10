#include "NinjaSwipeLayer.hpp"

NinjaSwipeLayer* NinjaSwipeLayer::create() {
    auto ret = new NinjaSwipeLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool NinjaSwipeLayer::init() {
    if (!CCLayer::init()) return false;

    // m_blade = CCBlade::createWithMaximumPoint(50);
    // m_blade->_texture = cocos2d::CCSprite::create("testswipe.png"_spr)->getTexture();
    // addChild(m_blade);

    // if (!m_blade->_texture) {
    //     geode::log::error("texture cache did not contain texture!!!!");
    //     return false;
    // }

    // stupid cocos touch
    setTouchEnabled(true);    
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, cocos2d::kCCMenuHandlerPriority, true);

    return true;
}

bool NinjaSwipeLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    m_lastSwipePoint = touch->getLocation();
    // m_blade->push(touch->getLocation());
    return true; // claim touch
}

void NinjaSwipeLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    checkSwipeIntersection(m_lastSwipePoint, touch->getLocation());
    // m_blade->push(touch->getLocation());
    m_lastSwipePoint = touch->getLocation();
}

void NinjaSwipeLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    checkSwipeIntersection(m_lastSwipePoint, touch->getLocation());
    // m_blade->push(touch->getLocation());
    // m_blade->finish();
}

void NinjaSwipeLayer::checkSwipeIntersection(const cocos2d::CCPoint& from, const cocos2d::CCPoint& to) {
    // TODO: if swipe is too short ignore
    // dont want people just clicking instead of swiping

    // get icon
    auto ml = MenuLayer::get();
    if (!ml) return;

    auto menuGameLayer = ml->m_menuGameLayer;
    if (!menuGameLayer) return; // idfk

    auto player = menuGameLayer->m_playerObject;
    if (!player) return; // idfk part 2

    // ok so this may be cheating
    // but instead of drawing a line i just get the last two points and place
    // 10 points spread out between them
    // and then check if any of them are inside the icon's hitbox

    // create points to test
    int pointNum = 20;
    cocos2d::CCPoint distanceBetweenPointSegments = (to - from) / pointNum;

    std::vector<cocos2d::CCPoint> points;
    for (int i = 0; i < pointNum; i++) {
        points.push_back(from + distanceBetweenPointSegments * i);
    }

    // check if any intersect icon
    auto rect = player->getObjectRect();
    bool intersectsAtLeastOnePoint = false;
    for (auto& point : points) {
        if (rect.containsPoint(point)) {
            // oh oh oh we've hit an icon
            intersectsAtLeastOnePoint = true;
            break;
        }
    }

    if (!intersectsAtLeastOnePoint) return;

    // ok so time to kill muahaha
    menuGameLayer->destroyPlayer();

    // TODO: combos idfk
}
