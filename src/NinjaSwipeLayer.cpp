#include "NinjaSwipeLayer.hpp"
#include "MenuLayer.hpp"
#include "utils/random.hpp"

NinjaSwipeLayer* NinjaSwipeLayer::create() {
    auto ret = new NinjaSwipeLayer;
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


    // add layers n shit
    auto dir = cocos2d::CCDirector::sharedDirector();

        // create exit button for gameplay
    m_exitButton = CCMenuItemSpriteExtra::create(
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_undoBtn_001.png"), // probably not the best sprite to use but whatever
        this, menu_selector(NinjaSwipeLayer::exitGameplay)
    );
    m_exitButton->setPosition({20.f, dir->getScreenTop() - 20.f + 50.f}); // +50 so its offscreen and can move down later
    auto menu = cocos2d::CCMenu::create();
    menu->setID("gameplay-exit-button"_spr);
    menu->addChild(m_exitButton);
    menu->setPosition({0, 0});
    addChild(menu);

    // create label for gameplay
    m_scoreLayer = cocos2d::CCLayerRGBA::create();
    m_scoreLayer->setCascadeOpacityEnabled(true);
    m_scoreLayer->setID("score-layer"_spr);
    m_scoreLayer->setPosition(dir->getWinSize() / 2);
    m_scoreLayer->setPositionY(m_scoreLayer->getPositionY() + 140.f);
    m_scoreLayer->runAction(cocos2d::CCFadeOut::create(0.f)); // setOpacity(0) doesnt work idfk please tell me why

    std::string fontString = "bigFont.fnt";
    int64_t font = geode::Mod::get()->getSettingValue<int64_t>("font");
    if (font > 1) {
        fontString = fmt::format("gjFont{:02}.fnt", font - 1);
    }

    m_comboLabel = cocos2d::CCLabelBMFont::create("...", fontString.c_str());
    m_comboLabel->setID("combo-label"_spr);
    m_scoreLayer->addChild(m_comboLabel);

    m_hiComboLabel = cocos2d::CCLabelBMFont::create("...", fontString.c_str());
    m_hiComboLabel->setID("hi-combo-label"_spr);
    m_hiComboLabel->setScale(.4f);
    m_hiComboLabel->setPositionY(-24.f);
    m_hiComboLabel->setOpacity(176);
    m_scoreLayer->addChild(m_hiComboLabel);

    addChild(m_scoreLayer);
    m_hiCombo = geode::Mod::get()->getSavedValue<int>("hi-combo", 0);
    updateComboShit();

    m_players.reserve(20); // performance! (not really)

    scheduleUpdate();


    return true;
}

bool NinjaSwipeLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    // by not clearing the point list before this adds the point causes the end
    // of the last swipe to connect to the next swipe but nobody is going to
    // notice it's fine
    if (m_isFingerDown) return false;
    m_isFingerDown = true;

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
    m_isFingerDown = false;

    checkSwipeIntersection(m_lastSwipePoint, touch->getLocation());
    m_swipe->addPoint(touch->getLocation());
}

void NinjaSwipeLayer::checkSwipeIntersection(const cocos2d::CCPoint& from, const cocos2d::CCPoint& to) {
    // dont let the player just click the icons
    if (from.getDistanceSq(to) < .5f) return;

    for (auto& player : m_players) {
        auto rect = player->getWorldBoundingBox();
        if (!geode::Mod::get()->getSettingValue<bool>("disable-margin")) {
            float margin = 5.f;
            rect.origin -= cocos2d::CCPoint{ margin, margin };
            rect.size += cocos2d::CCPoint{ margin, margin } * 2;
        }
        
        if (lineIntersectsRect(rect, from, to)) {
            if (player->m_type == MenuIconType::Bomb) {
                // oh no
                geode::log::error("TODO: KABLOOEY");
                if (m_state == State::Default) {
                    // ok nothing really

                } else {
                    // KABOOM
                    
                }
            } else {
                // ok so time to kill muahaha
                killPlayer(player);
            }
        }
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

void NinjaSwipeLayer::killPlayer(MenuIcon* player) {
    m_combo++;
    updateComboShit();

    geode::log::error("TODO: DEATH ANIMATIONS??");

    if (m_state == State::Gameplay && geode::Mod::get()->getSettingValue<bool>("enable-gameplay")) {
        enterGameplay();
    }
}

void NinjaSwipeLayer::update(float dt) {
    // physics
    std::vector<MenuIcon*> playersToRemove = {};
    for (auto player : m_players) {
        player->setPosition(player->getPosition() + (player->m_speed * dt));
        player->m_speed.y -= m_gravity * dt;
        player->setRotation(player->getRotation() + (player->m_speed.x * player->m_rotationSpeed * dt));

        auto dir = cocos2d::CCDirector::sharedDirector();
        if (player->getPositionY() <= -50.f || player->getPositionX() <= -50.f || player->getPositionX() >= dir->getScreenRight() + 50.f) {
            geode::log::info("icon fell offscreen :broken_heart:");
            playersToRemove.push_back(player);

            // offscreen, below starting point, remove and reset combo
            m_combo = 0;
            m_comboAnimationPlayed = false;
            updateComboShit();
            geode::Mod::get()->setSavedValue<int>("hi-combo", m_hiCombo);
        }
    }

    for (auto player : playersToRemove) {
        auto location = std::find(m_players.begin(), m_players.end(), player);
        m_players.erase(location);
        player->removeFromParent();
    }

    if (m_players.size() == 0 && !m_waitingForNextSpawn && !m_isSendingOutSpree) {
        geode::log::info("ran out! wait 1.5 secs, ill spawn some more");
        // ran out!
        auto action = cocos2d::CCSequence::createWithTwoActions(
            cocos2d::CCDelayTime::create(1.5f),
            geode::cocos::CallFuncExt::create([this]{
                m_waitingForNextSpawn = false;
                spawnPlayers();
            })
        );
        runAction(action);
        m_waitingForNextSpawn = true;
    }
}

void NinjaSwipeLayer::enterGameplay() {
    auto menuLayer = static_cast<HookedMenuLayer*>(MenuLayer::get());
    menuLayer->runEnterGameplayAnimations();

    m_scoreLayer->runAction(cocos2d::CCFadeIn::create(.9f));
    m_exitButton->runAction(cocos2d::CCSpawn::createWithTwoActions(
        cocos2d::CCEaseBackOut::create(cocos2d::CCMoveBy::create(.9f, {0.f, -50.f})),
        cocos2d::CCFadeIn::create(.9f)
    ));
}

void NinjaSwipeLayer::exitGameplay(cocos2d::CCObject* sender) {
    auto menuLayer = static_cast<HookedMenuLayer*>(MenuLayer::get());
    menuLayer->runExitGameplayAnimations();
    
    m_scoreLayer->runAction(cocos2d::CCFadeOut::create(.9f));
    m_exitButton->runAction(cocos2d::CCSpawn::createWithTwoActions(
        cocos2d::CCEaseBackOut::create(cocos2d::CCMoveBy::create(.9f, {0.f, 50.f})),
        cocos2d::CCFadeOut::create(.9f)
    ));
}

void NinjaSwipeLayer::updateComboShit() {
    if (m_combo > m_hiCombo) {
        // high score rn
        m_hiComboLabel->setColor({255, 213, 46});
        if (!m_comboAnimationPlayed) {
            m_comboAnimationPlayed = true;

            // literally just got a high score
            // make label "bounce" and rotate
            m_hiComboLabel->runAction(
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

        m_hiCombo = m_combo;
    } else {
        m_hiComboLabel->setColor({255, 255, 255}); // normal white
    }

    m_comboLabel->setString(fmt::format("Combo: {}", m_combo).c_str());
    m_hiComboLabel->setString(fmt::format("High Score: {}", m_hiCombo).c_str());
}


void NinjaSwipeLayer::spawnPlayers() {
    if (m_players.size() != 0) return;

    int type = ninja::random::spawnTypeDistribution(ninja::random::gen); // same height, spree, bomb one, bomb two, random one, random two, mix
    geode::log::info("spawning players (type: {})", type);
    switch(type) {
        case 0: {
            // all go to the same height
            float sharedHeight = ninja::random::launchSpeedYDistribution(ninja::random::gen);
            int count = ninja::random::playerSpawnDistribution(ninja::random::gen);
            for (int i = 0; i < count; i++) {
                auto player = MenuIcon::create(static_cast<MenuIconType>(ninja::random::menuIconTypeDistribution(ninja::random::gen)));
                player->m_speed.y = sharedHeight;
                m_players.push_back(player);
            }
            break;
        }

        case 1: {
            // spree, bunch get sent out randomly in quick succession
            int count = ninja::random::spreeIconCountDistribution(ninja::random::gen);
            m_isSendingOutSpree = true;

            // alright guys lets cocos action this up
            auto spawnSequence = cocos2d::CCSequence::createWithTwoActions(
                geode::cocos::CallFuncExt::create([this]{
                    auto icon = MenuIcon::create(MenuIconType::Player);
                    m_players.push_back(icon);
                    // usually done at the end of this function (adds all of m_players)
                    // but cant for obvious reasons
                    addChild(icon);
                }),
                cocos2d::CCDelayTime::create(.35f)
            );

            auto mainSequence = cocos2d::CCSequence::createWithTwoActions(
                cocos2d::CCRepeat::create(spawnSequence, count),
                geode::cocos::CallFuncExt::create([this]{
                    m_isSendingOutSpree = false;
                })
            );

            runAction(mainSequence);
            break;
        }

        case 2:
        case 3: {
            // random # of bombs
            int count = ninja::random::bombSpawnDistribution(ninja::random::gen);
            for (int i = 0; i < count; i++) {
                m_players.push_back(MenuIcon::create(MenuIconType::Bomb));
            }
            break;
        }

        case 4:
        case 5: {
            // random # of icons
            int count = ninja::random::playerSpawnDistribution(ninja::random::gen);
            for (int i = 0; i < count; i++) {
                m_players.push_back(MenuIcon::create(MenuIconType::Player));
            }
            break;
        }

        case 6: {
            // random mix
            int count = ninja::random::mixIconCountDistribution(ninja::random::gen);
            for (int i = 0; i < count; i++) {
                m_players.push_back(MenuIcon::create(static_cast<MenuIconType>(ninja::random::menuIconTypeDistribution(ninja::random::gen))));
            }
            break;
        }
    }

    for (auto player : m_players) {
        addChild(player);
    }
}

$on_mod(Loaded) {
    geode::log::info("hi spaghett ive removed the logs dw");
}
