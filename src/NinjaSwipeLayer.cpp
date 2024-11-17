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
    cocos2d::CCTouchDispatcher::get()->addTargetedDelegate(this, cocos2d::kCCMenuHandlerPriority, true);

    return true;
}

bool NinjaSwipeLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    // by not clearing the point list before this adds the point causes the end
    // of the last swipe to connect to the next swipe but nobody is going to
    // notice it's fine

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
    // dont let the player just click the icons
    if (from.getDistanceSq(to) < .5f) return;

    // get icon
    auto ml = MenuLayer::get();
    if (!ml) return;

    auto menuGameLayer = ml->m_menuGameLayer;
    if (!menuGameLayer) return;

    auto player = menuGameLayer->m_playerObject;
    if (!player) return;

    // broken vtables so have to do this shit
    auto rect = player->GameObject::getObjectRect();
    if (!geode::Mod::get()->getSettingValue<bool>("disable-margin")) {
        float margin = 5.f;
        rect.origin -= cocos2d::CCPoint{ margin, margin };
        rect.size += cocos2d::CCPoint{ margin, margin } * 2;
    }
    
    if (lineIntersectsRect(rect, from, to)) {
        // ok so time to kill muahaha
        menuGameLayer->destroyPlayer();
    }
}

// code courtesy of yours truly, gpt-4o (idk wtf half of this means)
// The algorithm used here is a Line-AABB Intersection Algorithm.
// It's commonly implemented using the Cohen-Sutherland clipping or the
// Liang-Barsky algorithm principles, but in this specific case, it's closer to
// the Liang-Barsky algorithm because it uses parametric line equations to
// calculate intersection intervals.
bool NinjaSwipeLayer::lineIntersectsRect(const cocos2d::CCRect& rect, const cocos2d::CCPoint& p1, const cocos2d::CCPoint& p2) {
    if (rect.containsPoint(p1) || rect.containsPoint(p2)) return true;

    auto lineIntersect = [](float p1, float p2, float min, float max, float& t1, float& t2) {
        float d = p2 - p1;
        if (fabs(d) < 1e-6) return p1 >= min && p1 <= max; // Line parallel to the edge
        float tMin = (min - p1) / d;
        float tMax = (max - p1) / d;
        if (tMin > tMax) std::swap(tMin, tMax);
        t1 = std::max(t1, tMin);
        t2 = std::min(t2, tMax);
        return t1 <= t2;
    };

    float t1 = 0.0f, t2 = 1.0f;
    if (!lineIntersect(p1.x, p2.x, rect.getMinX(), rect.getMaxX(), t1, t2)) return false;
    if (!lineIntersect(p1.y, p2.y, rect.getMinY(), rect.getMaxY(), t1, t2)) return false;
    return true;
}
