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

    m_hardStreak = HardStreak::create();
    addChild(m_hardStreak);

    // stupid cocos touch
    setTouchEnabled(true);    
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, cocos2d::kCCMenuHandlerPriority, true);

    return true;
}

bool NinjaSwipeLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    m_lastTouchPoint = touch->getLocation();
    return true; // claim touch
}

void NinjaSwipeLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    cocos2d::CCPoint point = touch->getLocation();
    m_hardStreak->addPoint(point); // TODO: fix this shit hardstreak does not work why
    checkSwipeIntersection(point, m_lastTouchPoint);
    m_lastTouchPoint = point;
}

void NinjaSwipeLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    cocos2d::CCPoint point = touch->getLocation();
    m_hardStreak->addPoint(point);
    checkSwipeIntersection(point, m_lastTouchPoint);
    m_hardStreak->reset();
    // no need to reset m_lastTouchPoint since it gets reset in ccTouchBegan anyway
}

void NinjaSwipeLayer::checkSwipeIntersection(cocos2d::CCPoint& from, cocos2d::CCPoint& last) {
    geode::log::info("check swipe intersection");

    // get icon
    auto ml = MenuLayer::get();
    if (!ml) return;

    auto menuGameLayer = ml->m_menuGameLayer;
    if (!menuGameLayer) return; // idfk

    auto player = menuGameLayer->m_playerObject;
    if (!player) return; // idfk part 2

    // ok so this may be cheating
    // but instead of drawing a line i just get the last two points and place
    // 6 points spread out between them
    // and then check if any of them are inside the icon's hitbox

    // create points to test (incl. start and end)
    int pointNum = 6;
    cocos2d::CCPoint distanceBetweenPointSegments = (from - last) / pointNum;

    std::vector<cocos2d::CCPoint> points;
    points.push_back(last); // last point checked first
    for (int i = 0; i < pointNum; i++) {
        points.push_back(distanceBetweenPointSegments * i);
    }
    points.push_back(from);

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

    geode::log::info("kil");

    // ok so time to kill muahaha
    menuGameLayer->destroyPlayer();

    // TODO: combos idfk
}
