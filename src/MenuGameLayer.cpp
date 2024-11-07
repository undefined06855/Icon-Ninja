#include "MenuGameLayer.hpp"
#include "NinjaSwipeLayer.hpp"

bool HookedMenuGameLayer::init() {
    if (!MenuGameLayer::init()) return false;

    std::random_device seed;
    m_fields->gen = std::mt19937(seed());
    m_fields->startXDistribution = std::uniform_real_distribution<float>(130.f, getContentWidth() - 130.f);
    m_fields->launchSpeedXDistribution = std::uniform_real_distribution<float>(-200.f, 200.f); 
    m_fields->launchSpeedYDistribution = std::uniform_real_distribution<float>(360.0, 380.f); 
    m_fields->rotationSpeedDistribution = std::uniform_real_distribution<float>(-20.f, 20.f); 

    // add custom layer to detect touch and draw the swipe
    geode::log::info("add");
    auto swipeLayer = NinjaSwipeLayer::create();
    swipeLayer->setZOrder(999);
    addChild(swipeLayer);

    // remove touch from this
    setTouchEnabled(false);

    // remove ground
    if (geode::Mod::get()->getSettingValue<bool>("custom-player-movement"))
        m_groundLayer->setVisible(false);

    return true;
}

void HookedMenuGameLayer::update(float dt) {
    MenuGameLayer::update(dt);

    if (!geode::Mod::get()->getSettingValue<bool>("custom-player-movement")) return;
    if (m_playerObject->m_isDead) return geode::log::info("player is dead!");

    // physics

    m_fields->playerPos += m_fields->playerSpeed * dt;
    m_fields->playerSpeed.y -= m_fields->gravity * dt;
    m_fields->playerRotation += m_fields->playerSpeed.x * m_fields->playerRotationSpeed * dt;

    m_playerObject->setPosition(m_fields->playerPos);
    m_playerObject->setRotation(m_fields->playerRotation);

    if (m_fields->playerPos.y <= -50.f || m_fields->playerPos.x <= -50.f || m_fields->playerPos.x >= getContentWidth() + 50.f) {
        // offscreen, below starting point, reset
        resetPlayer();
    }
}

void HookedMenuGameLayer::resetPlayer() {
    MenuGameLayer::resetPlayer();

    m_fields->playerPos.x = getRandomXPos();
    m_fields->playerPos.y = -40.f;
    m_fields->playerSpeed = getRandomLaunchSpeed();
    m_fields->playerRotationSpeed = getRandomRotationSpeed();

    m_playerObject->m_playerGroundParticles->setScale(0.f);

    // make player move towards the middle
    if (m_fields->playerPos.x 
}

float HookedMenuGameLayer::getRandomXPos() {
    return m_fields->startXDistribution(m_fields->gen);
}

float HookedMenuGameLayer::getRandomRotationSpeed() {
    return m_fields->rotationSpeedDistribution(m_fields->gen);
}

cocos2d::CCPoint HookedMenuGameLayer::getRandomLaunchSpeed() {
    return {
        m_fields->launchSpeedXDistribution(m_fields->gen),
        m_fields->launchSpeedYDistribution(m_fields->gen)
    };
}
