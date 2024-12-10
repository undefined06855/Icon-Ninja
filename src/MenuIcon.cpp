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
        m_bombSprite->setID("bomb-icon");
        addChild(m_bombSprite);

        if (geode::Mod::get()->getSettingValue<bool>("particles")) {
            m_particles = GameToolbox::particleFromString("20a-1a0.68a0.67a-1a90a17a98a0a0a0a13a-200a57a0a0a0a4a1a0a360a1a0a1a0a0a0a1a0a1a1a360a0a1a0a0a0a0a0a1a0a0.03a0a0.28a0a0a0a0a0a0a0a0a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0", nullptr, false);
            m_particles->setPosition({-21.f, 20.f});
            m_particles->setRotation(-98);
            m_particles->setZOrder(-1);
            m_particles->setID("particles");
            m_particles->setPositionType(cocos2d::kCCPositionTypeFree);
            addChild(m_particles);
        }
    } else {
        m_playerObject = ninja::random::createRandomPlayerObject();
        m_playerObject->setID("player-icon");
        addChild(m_playerObject);
    }

    initialiseValues();

    return true;
}

void MenuIcon::initialiseValues() {
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

cocos2d::CCPoint MenuIcon::getWorldPos() {
    auto bound = boundingBox();
    
    // convert to world space
    cocos2d::CCPoint worldPos = convertToWorldSpace(getPosition());

    // convert to node space relative to swipeLayer
    auto swipeLayer = static_cast<HookedMenuGameLayer*>(MenuLayer::get()->m_menuGameLayer)->m_fields->ninjaSwipeLayer;

    return swipeLayer->convertToNodeSpace(worldPos);
}

float MenuIcon::getRadius() {
    if (m_type == MenuIconType::Bomb) return 15.f;
    return m_playerObject->m_vehicleSize * 16.f;
}
