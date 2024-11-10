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

    auto dir = cocos2d::CCDirector::sharedDirector();

    // create exit button for gameplay
    fields->exitButton = CCMenuItemSpriteExtra::create(
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_undoBtn_001.png"), // probably not the best sprite to use but whatever
        this, menu_selector(HookedMenuGameLayer::exitGameplay)
    );
    fields->exitButton->setPosition({dir->getScreenLeft() + 20.f, dir->getScreenTop() - 20.f + 50.f}); // +50 so its offscreen and can move down later
    auto menu = cocos2d::CCMenu::create();
    menu->setID("gameplay-exit-button"_spr);
    menu->addChild(fields->exitButton);
    menu->setPosition({0, 0});
    addChild(menu);

    // create label for gameplay
    fields->scoreLayer = cocos2d::CCLayer::create();
    fields->scoreLayer->setID("score-layer"_spr);
    fields->scoreLayer->setPosition(dir->getWinSize() / 2);
    fields->scoreLayer->setPositionY(fields->scoreLayer->getPositionY() + 140.f);
    fields->scoreLayer->setOpacity(0.f);

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

    updateComboShit();

    // everything below here isn't needed if this is normal player movement
    if (!geode::Mod::get()->getSettingValue<bool>("custom-player-movement")) return true;

    std::random_device seed;
    fields->gen = std::mt19937(seed());
    fields->startXDistribution = std::uniform_real_distribution<float>(130.f, getContentWidth() - 130.f);
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

    if (fields->playerPos.y <= -50.f || fields->playerPos.x <= -50.f || fields->playerPos.x >= getContentWidth() + 50.f) {
        // offscreen, below starting point, reset
        resetPlayer();
        if (fields->combo > fields->hiCombo) fields->hiCombo = fields->combo;
        fields->combo = 0;
        updateComboShit();
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
    if (fields->playerPos.x > getContentWidth() / 2) {
        // starts on the right, speed should be negative
        fields->playerSpeed.x = -fields->playerSpeed.x;
    }
}

void HookedMenuGameLayer::destroyPlayer() {
    MenuGameLayer::destroyPlayer();
    m_fields->combo++;
    updateComboShit();

    if (!m_fields->isInGameplay && geode::Mod::get()->getSettingValue<bool>("enable-gameplay")) {
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
        fields->hiComboLabel->setColor({255, 213, 46}); // yellow
        
        fields->displayHiCombo = fields->combo;

        if (fields->hiCombo + 1 == fields->combo) {
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
    } else {
        fields->hiComboLabel->setColor({255, 255, 255}); // normal white
    }

    fields->comboLabel->setString(fmt::format("Combo: {}", fields->combo).c_str());
    fields->hiComboLabel->setString(fmt::format("High Score: {}", fields->displayHiCombo).c_str());
}

#define FADE_OUT_ACTION(x, y) cocos2d::CCSpawn::createWithTwoActions(cocos2d::CCEaseBackIn::create(cocos2d::CCMoveBy::create(.9f, {x, y})), cocos2d::CCFadeOut::create(.9f))
#define FADE_IN_ACTION(x, y) cocos2d::CCSpawn::createWithTwoActions(cocos2d::CCEaseBackOut::create(cocos2d::CCMoveBy::create(.9f, {-x, -y})), cocos2d::CCFadeIn::create(.9f))

void HookedMenuGameLayer::runEnterGameplayAnimations() {
    auto ml = getParent();

    auto buttons = ml->getChildByID("main-menu");
    if (buttons) buttons->runAction(FADE_OUT_ACTION(0, 120));

    auto bottom = ml->getChildByID("bottom-menu");
    if (bottom) bottom->runAction(FADE_OUT_ACTION(0, -90));

    auto profile = ml->getChildByID("profile-menu");
    if (profile) profile->runAction(FADE_OUT_ACTION(-182, 0));
    auto username = ml->getChildByID("player-username");
    if (username) username->runAction(FADE_OUT_ACTION(-182, 0));

    auto right = ml->getChildByID("right-side-menu");
    if (right) right->runAction(FADE_OUT_ACTION(87, 0));

    auto topRight = ml->getChildByID("top-right-menu");
    if (topRight) topRight->runAction(FADE_OUT_ACTION(203, 58));

    auto side = ml->getChildByID("side-menu");
    if (side) side->runAction(FADE_OUT_ACTION(-50, 0));

    auto title = ml->getChildByID("main-title");
    if (title) title->runAction(FADE_OUT_ACTION(0, 120));

    auto social = ml->getChildByID("social-media-menu");
    if (social) social->runAction(FADE_OUT_ACTION(-107, -52));

    auto moreGames = ml->getChildByID("more-games-menu");
    if (moreGames) moreGames->runAction(FADE_OUT_ACTION(118, -50));

    auto close = ml->getChildByID("close-menu");
    if (close) close->runAction(FADE_OUT_ACTION(-202, 40));

    m_fields->scoreLayer->runAction(cocos2d::CCFadeIn::create(.9f));
    m_fields->exitButton->runAction(cocos2d::CCSpawn::createWithTwoActions(
        cocos2d::CCEaseBackOut::create(cocos2d::CCMoveBy::create(.9f, {0.f, -50.f})),
        cocos2d::CCFadeIn::create(.9f)
    ));
}

void HookedMenuGameLayer::runExitGameplayAnimations() {
    auto ml = getParent();

    auto buttons = ml->getChildByID("main-menu");
    if (buttons) buttons->runAction(FADE_IN_ACTION(0, 120));

    auto bottom = ml->getChildByID("bottom-menu");
    if (bottom) bottom->runAction(FADE_IN_ACTION(0, -90));

    auto profile = ml->getChildByID("profile-menu");
    if (profile) profile->runAction(FADE_IN_ACTION(-182, 0));
    auto username = ml->getChildByID("player-username");
    if (username) username->runAction(FADE_IN_ACTION(-182, 0));

    auto right = ml->getChildByID("right-side-menu");
    if (right) right->runAction(FADE_IN_ACTION(87, 0));

    auto topRight = ml->getChildByID("top-right-menu");
    if (topRight) topRight->runAction(FADE_IN_ACTION(203, 58));

    auto side = ml->getChildByID("side-menu");
    if (side) side->runAction(FADE_IN_ACTION(-50, 0));

    auto title = ml->getChildByID("main-title");
    if (title) title->runAction(FADE_IN_ACTION(0, 120));

    auto social = ml->getChildByID("social-media-menu");
    if (social) social->runAction(FADE_IN_ACTION(-107, -52));

    auto moreGames = ml->getChildByID("more-games-menu");
    if (moreGames) moreGames->runAction(FADE_IN_ACTION(118, -50));

    auto close = ml->getChildByID("close-menu");
    if (close) close->runAction(FADE_IN_ACTION(-202, 40));

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
}

void HookedMenuGameLayer::exitGameplay(CCObject* sender) {
    if (!m_fields->isInGameplay) return;

    m_fields->isInGameplay = false;
    geode::log::info("no more ninja time :broken_heart:");
    runExitGameplayAnimations();

    // fade in music
    FMODAudioEngine::sharedEngine()->fadeInMusic(1.f, 0);
}
