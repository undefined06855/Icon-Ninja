#include "MenuIcon.hpp"
#include "MenuGameLayer.hpp"
#include "utils/random.hpp"

MenuIcon* MenuIcon::create(MenuIconType type) {
    auto ret = new MenuIcon;
    if (ret->init(type)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool MenuIcon::init(MenuIconType type) {
    m_type = type;

    if (type == MenuIconType::Bomb) {
        m_bombSprite = cocos2d::CCSprite::create("bomb.png"_spr);
        m_bombSprite->setScale(.7f);
        addChild(m_bombSprite);
    } else {
        auto layer = cocos2d::CCLayer::create();
        m_playerObject = PlayerObject::create(1, 1, nullptr, layer, false);
        addChild(m_playerObject);
        addChild(layer); // not sure what this is for yet, let's see later
    }

    initialiseValues();

    return true;
}

void MenuIcon::initialiseValues() {
    if (m_type == MenuIconType::Player) {
        static_cast<HookedMenuGameLayer*>(MenuLayer::get()->m_menuGameLayer)->randomisePlayerObject(m_playerObject);
    }

    setPosition({
        ninja::random::startXDistribution(ninja::random::gen),
        -40.f
    });

    float launchSpeedX = ninja::random::launchSpeedXDistribution(ninja::random::gen);
    if (getPositionX() > cocos2d::CCDirector::sharedDirector()->getScreenRight()/2) {
        // go to the left if it starts on the right
        launchSpeedX = -launchSpeedX;
    }

    m_speed = cocos2d::CCPoint{
        launchSpeedX,
        ninja::random::launchSpeedYDistribution(ninja::random::gen)
    };

    m_rotationSpeed = ninja::random::rotationSpeedDistribution(ninja::random::gen);
}

cocos2d::CCRect MenuIcon::boundingBox() {
    if (m_type == MenuIconType::Bomb) {
        return m_bombSprite->boundingBox();
    } else {
        return m_playerObject->GameObject::getObjectRect();
    }
}

cocos2d::CCRect MenuIcon::getWorldBoundingBox() {
    auto bound = boundingBox();
    
    // convert to world space
    cocos2d::CCPoint bottomLeft = convertToWorldSpace({bound.getMinX(), bound.getMinY()});
    cocos2d::CCPoint topRight = convertToWorldSpace({bound.getMaxX(), bound.getMaxY()});

    // convert to node space relative to swipeLayer
    auto swipeLayer = static_cast<HookedMenuGameLayer*>(MenuLayer::get()->m_menuGameLayer)->m_fields->ninjaSwipeLayer;

    cocos2d::CCPoint bottomLeft2 = swipeLayer->convertToNodeSpace(bottomLeft);
    cocos2d::CCPoint topRight2 = swipeLayer->convertToNodeSpace(topRight);

    cocos2d::CCRect ret;

    ret.origin = bottomLeft2;
    ret.size = topRight2 - bottomLeft2;

    return ret;
}
