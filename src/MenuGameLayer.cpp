#include "MenuGameLayer.hpp"
#include "NinjaSwipeLayer.hpp"

bool HookedMenuGameLayer::init() {
    if (!MenuGameLayer::init()) return false;

    auto fields = m_fields.self();

    // add custom layer to detect touch and draw the swipe
    auto swipeLayer = NinjaSwipeLayer::create();
    swipeLayer->setZOrder(999);
    addChild(swipeLayer);
    // remove touch from this
    setTouchEnabled(false);

    setKeyboardEnabled(true);

    auto dir = cocos2d::CCDirector::sharedDirector();

    // create exit button for gameplay
    fields->exitButton = CCMenuItemSpriteExtra::create(
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_undoBtn_001.png"), // probably not the best sprite to use but whatever
        this, menu_selector(HookedMenuGameLayer::exitGameplay)
    );
    fields->exitButton->setPosition({20.f, dir->getScreenTop() - 20.f + 50.f}); // +50 so its offscreen and can move down later
    auto menu = cocos2d::CCMenu::create();
    menu->setID("gameplay-exit-button"_spr);
    menu->addChild(fields->exitButton);
    menu->setPosition({0, 0});
    addChild(menu);

    // create label for gameplay
    fields->scoreLayer = cocos2d::CCLayerRGBA::create();
    fields->scoreLayer->setCascadeOpacityEnabled(true);
    fields->scoreLayer->setID("score-layer"_spr);
    fields->scoreLayer->setPosition(dir->getWinSize() / 2);
    fields->scoreLayer->setPositionY(fields->scoreLayer->getPositionY() + 140.f);
    fields->scoreLayer->runAction(cocos2d::CCFadeOut::create(0.f)); // setOpacity(0) doesnt work idfk please tell me why

    fields->comboLabel = cocos2d::CCLabelBMFont::create("...", "gjFont31.fnt");
    fields->comboLabel->setID("combo-label"_spr);
    fields->scoreLayer->addChild(fields->comboLabel);

    fields->hiComboLabel = cocos2d::CCLabelBMFont::create("...", "gjFont31.fnt");
    fields->hiComboLabel->setID("hi-combo-label"_spr);
    fields->hiComboLabel->setScale(.4f);
    fields->hiComboLabel->setPositionY(-24.f);
    fields->hiComboLabel->setOpacity(176);
    fields->scoreLayer->addChild(fields->hiComboLabel);

    addChild(fields->scoreLayer);
    fields->hiCombo = geode::Mod::get()->getSavedValue<int>("hi-combo", 0);
    updateComboShit();

    // everything below here isn't needed if this is normal player movement
    if (!geode::Mod::get()->getSettingValue<bool>("custom-player-movement")) return true;

    std::random_device seed;
    fields->gen = std::mt19937(seed());
    fields->startXDistribution = std::uniform_real_distribution<float>(70.f, dir->getScreenRight() - 70.f);
    fields->launchSpeedXDistribution = std::uniform_real_distribution<float>(0.f, 200.f); // could become 0 to -200 depending on start side
    fields->launchSpeedYDistribution = std::uniform_real_distribution<float>(360.0, 380.f); 
    fields->rotationSpeedDistribution = std::uniform_real_distribution<float>(-6.f, 6.f); 

    // remove shit
    if (geode::Mod::get()->getSettingValue<bool>("custom-player-movement")) {
        m_groundLayer->setVisible(false);
        getChildByType<HardStreak>(0)->setVisible(false);
    }

    return true;
}

void HookedMenuGameLayer::update(float dt) {
    MenuGameLayer::update(dt);

    if (!geode::Mod::get()->getSettingValue<bool>("custom-player-movement")) return;
    if (m_playerObject->m_isDead) return;

    // physics
    auto fields = m_fields.self();

    fields->playerPos += fields->playerSpeed * dt;
    fields->playerSpeed.y -= fields->gravity * dt;
    fields->playerRotation += fields->playerSpeed.x * fields->playerRotationSpeed * dt;

    m_playerObject->setPosition(fields->playerPos);
    m_playerObject->setRotation(fields->playerRotation);

    auto dir = cocos2d::CCDirector::sharedDirector();
    if (fields->playerPos.y <= -50.f || fields->playerPos.x <= -50.f || fields->playerPos.x >= dir->getScreenRight() + 50.f) {
        // offscreen, below starting point, reset
        resetPlayer();
        fields->combo = 0;
        fields->comboAnimationPlayed = false;
        updateComboShit();
        geode::Mod::get()->setSavedValue<int>("hi-combo", fields->hiCombo);
    }
}

void HookedMenuGameLayer::resetPlayer() {
    MenuGameLayer::resetPlayer();

    if (!geode::Mod::get()->getSettingValue<bool>("custom-player-movement")) return;

    auto fields = m_fields.self();
    fields->playerPos.x = getRandomXPos();
    fields->playerPos.y = -40.f;
    fields->playerSpeed = getRandomLaunchSpeed();
    fields->playerRotationSpeed = getRandomRotationSpeed();

    // make player move towards the middle
    // by default speed is positive, so if the icon starts on the left it's fine
    auto dir = cocos2d::CCDirector::sharedDirector();
    if (fields->playerPos.x > dir->getScreenRight() / 2) {
        // starts on the right, speed should be negative
        fields->playerSpeed.x = -fields->playerSpeed.x;
    }
}

void HookedMenuGameLayer::destroyPlayer() {
    MenuGameLayer::destroyPlayer();

    if (!geode::Mod::get()->getSettingValue<bool>("custom-player-movement")) return;
    if (!geode::Mod::get()->getSettingValue<bool>("enable-gameplay")) return;

    m_fields->combo++;
    updateComboShit();

    if (!m_fields->isInGameplay && geode::Mod::get()->getSettingValue<bool>("enable-gameplay") && geode::Mod::get()->getSettingValue<bool>("custom-player-movement")) {
        enterGameplay();
    }
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


// gameplay stuff ---------------------------------------
void HookedMenuGameLayer::updateComboShit() {
    auto fields = m_fields.self();

    if (fields->combo > fields->hiCombo) {
        // high score rn
        fields->hiComboLabel->setColor({255, 213, 46});
        if (!fields->comboAnimationPlayed) {
            fields->comboAnimationPlayed = true;

            // literally just got a high score
            // make label "bounce" and rotate
            fields->hiComboLabel->runAction(
                cocos2d::CCSequence::createWithTwoActions(
                    cocos2d::CCSpawn::createWithTwoActions(
                        cocos2d::CCEaseOut::create(cocos2d::CCScaleBy::create(.23f, 1.4f), 2.f),
                        cocos2d::CCEaseOut::create(cocos2d::CCRotateBy::create(.23f, 5.f), 2.f)
                    ),
                    cocos2d::CCSpawn::createWithTwoActions(
                        cocos2d::CCEaseIn ::create(cocos2d::CCScaleBy::create(.23f, 1.f/1.4f), 2.f),
                        cocos2d::CCEaseIn ::create(cocos2d::CCRotateBy::create(.23f, -5.f), 2.f)
                    )
                )
            );
        }

        fields->hiCombo = fields->combo;
    } else {
        fields->hiComboLabel->setColor({255, 255, 255}); // normal white
    }

    fields->comboLabel->setString(fmt::format("Combo: {}", fields->combo).c_str());
    fields->hiComboLabel->setString(fmt::format("High Score: {}", fields->hiCombo).c_str());
}

#define FADE_OUT_ACTION(movement) cocos2d::CCSpawn::createWithTwoActions(cocos2d::CCEaseBackIn::create(cocos2d::CCMoveBy::create(.9f, movement)), cocos2d::CCFadeOut::create(.9f))
#define FADE_IN_ACTION(movement) cocos2d::CCSpawn::createWithTwoActions(cocos2d::CCEaseBackOut::create(cocos2d::CCMoveBy::create(.9f, -movement)), cocos2d::CCFadeIn::create(.9f))

void HookedMenuGameLayer::runEnterGameplayAnimations() {
    auto ml = getParent();
    auto dir = cocos2d::CCDirector::sharedDirector();
    m_fields->movedNodeMovements.clear();

    for (auto& child : geode::cocos::CCArrayExt<CCNode*>(ml->getChildren())) {
        if (child == this) continue;

        // get side + move dist
        cocos2d::CCPoint movement;

        if (child->getPositionX() == dir->getScreenRight()/2) {
            // middle
            if (child->getPositionY() < dir->getScreenTop()/2) {
                // bottom middle
                movement = cocos2d::CCPoint{0.f, -child->getPositionY() - child->getContentHeight()};
            } else {
                // top middle (default if in centre)
                movement = cocos2d::CCPoint{0.f, dir->getScreenTop() - child->getPositionY() + child->getContentHeight()};
            }
        } else if (child->getPositionX() < dir->getScreenRight()/2) {
            // left
            movement = cocos2d::CCPoint{-child->getPositionX() - child->getContentWidth(), 0.f};
        } else {
            // right
            movement = cocos2d::CCPoint{(dir->getScreenRight() - child->getPositionX()) + child->getContentWidth(), 0.f};
        }

        child->runAction(FADE_OUT_ACTION(movement));
        m_fields->movedNodeMovements[child] = movement;
    }

    m_fields->scoreLayer->runAction(cocos2d::CCFadeIn::create(.9f));
    m_fields->exitButton->runAction(cocos2d::CCSpawn::createWithTwoActions(
        cocos2d::CCEaseBackOut::create(cocos2d::CCMoveBy::create(.9f, {0.f, -50.f})),
        cocos2d::CCFadeIn::create(.9f)
    ));
}

void HookedMenuGameLayer::runExitGameplayAnimations() {
    for (auto& pair : m_fields->movedNodeMovements) {
        auto& child = pair.first;
        auto movement = pair.second;
        child->runAction(FADE_IN_ACTION(movement));
    }

    m_fields->scoreLayer->runAction(cocos2d::CCFadeOut::create(.9f));
    m_fields->exitButton->runAction(cocos2d::CCSpawn::createWithTwoActions(
        cocos2d::CCEaseBackOut::create(cocos2d::CCMoveBy::create(.9f, {0.f, 50.f})),
        cocos2d::CCFadeIn::create(.9f)
    ));
}

void HookedMenuGameLayer::enterGameplay() {
    if (m_fields->isInGameplay) return;

    m_fields->isInGameplay = true;
    geode::log::info("NINJA TIME BABY");
    runEnterGameplayAnimations();

    // fade out music
    FMODAudioEngine::sharedEngine()->fadeOutMusic(1.f, 0);
    FMODAudioEngine::sharedEngine()->playEffect("gamestart.wav"_spr);
}

void HookedMenuGameLayer::exitGameplay(CCObject* sender) {
    if (!m_fields->isInGameplay) return;

    m_fields->isInGameplay = false;
    geode::log::info("no more ninja time :broken_heart:");
    runExitGameplayAnimations();

    // fade in music
    FMODAudioEngine::sharedEngine()->fadeInMusic(1.f, 0);
    FMODAudioEngine::sharedEngine()->playEffect("gameover.wav"_spr);
}
