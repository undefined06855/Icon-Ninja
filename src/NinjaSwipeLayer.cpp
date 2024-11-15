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

    auto spr = cocos2d::CCSprite::create("swipe.png"_spr);
    m_swipe = Swipe::create(spr->getTexture());
    addChild(m_swipe);

    // stupid cocos touch 
    setTouchEnabled(true);    
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, cocos2d::kCCMenuHandlerPriority, true);

    return true;
}

bool NinjaSwipeLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    m_lastSwipePoint = touch->getLocation();
    m_swipe->addPoint(touch->getLocation());
    return true; // claim touch
}

void NinjaSwipeLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    checkSwipeIntersection(m_lastSwipePoint, touch->getLocation());
    m_swipe->addPoint(touch->getLocation());
    m_lastSwipePoint = touch->getLocation();
}

void NinjaSwipeLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    checkSwipeIntersection(m_lastSwipePoint, touch->getLocation());
    m_swipe->addPoint(touch->getLocation());
}

void NinjaSwipeLayer::checkSwipeIntersection(const cocos2d::CCPoint& from, const cocos2d::CCPoint& to) {
    // TODO: if swipe is too short ignore
    // dont want people just clicking instead of swiping

    // get icon
    auto ml = MenuLayer::get();
    if (!ml) return;

    auto menuGameLayer = ml->m_menuGameLayer;
    if (!menuGameLayer) return;

    auto player = menuGameLayer->m_playerObject;
    if (!player) return;

    // ok so this may be cheating
    // but instead of drawing a line i just get the last two points and place
    // 20 points spread out between them
    // and then check if any of them are inside the icon's hitbox

    // create points to test
    int pointNum = 20;
    cocos2d::CCPoint distanceBetweenPointSegments = (to - from) / pointNum;

    std::vector<cocos2d::CCPoint> points;
    for (int i = 0; i < pointNum; i++) {
        points.push_back(from + distanceBetweenPointSegments * i);
    }

    // check if any intersect icon
    // see https://discord.com/channels/911701438269386882/911702535373475870/1306658231854170246
    // broken vtable remember to update once it gets fixed
    auto rect = player->GameObject::getObjectRect();
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
}
