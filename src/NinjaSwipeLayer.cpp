#include "NinjaSwipeLayer.hpp"
#include <Geode/cocos/actions/CCActionInstant.h>
#include <Geode/cocos/actions/CCActionInterval.h>
#include <Geode/cocos/label_nodes/CCLabelBMFont.h>
#include <Geode/binding/FMODAudioEngine.hpp>
#include <Geode/binding/CCLightFlash.hpp>
#include <Geode/binding/GameStatsManager.hpp>
#include <Geode/binding/CCCircleWave.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <Geode/binding/GameManager.hpp>
#include "MenuIcon.hpp"
#include "hooks/MenuLayer.hpp"
#include "utils/random.hpp"
#include "utils/log.hpp"
#include "FlashbangLayer.hpp"
#include "CCBrighten.hpp"

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
    setTouchMode(cocos2d::kCCTouchesAllAtOnce);

    // add layers n shit
    auto dir = cocos2d::CCDirector::sharedDirector();

    // create exit button for gameplay
    m_exitButton = CCMenuItemSpriteExtra::create(
        cocos2d::CCSprite::createWithSpriteFrameName("GJ_undoBtn_001.png"), // probably not the best sprite to use but whatever
        this, menu_selector(NinjaSwipeLayer::exitGameplay)
    );
    m_exitButton->setPosition({20.f, dir->getScreenTop() - 20.f + 50.f}); // +50 so its offscreen and can move down later
    auto menu = cocos2d::CCMenu::create();
    menu->setID("gameplay-exit-button");
    menu->addChild(m_exitButton);
    menu->setPosition({0, 0});
    addChild(menu);

    // create label for gameplay
    m_scoreLayer = cocos2d::CCLayerRGBA::create();
    m_scoreLayer->setCascadeOpacityEnabled(true);
    m_scoreLayer->setID("score-layer");
    m_scoreLayer->setPosition(dir->getWinSize() / 2);
    m_scoreLayer->setPositionY(m_scoreLayer->getPositionY() + 140.f);
    m_scoreLayer->runAction(cocos2d::CCFadeOut::create(0.f)); // setOpacity(0) doesnt work idfk please tell me why
    m_scoreLayer->setOpacity(0); // for the platforms where it does (also it might have fixed itself maybe)

    std::string fontString = "bigFont.fnt";
    int64_t font = geode::Mod::get()->getSettingValue<int64_t>("font");
    if (font > 1) {
        // @geode-ignore(unknown-resource)
        fontString = fmt::format("gjFont{:02}.fnt", font - 1);
    }

    m_comboLabel = cocos2d::CCLabelBMFont::create("...", fontString.c_str());
    m_comboLabel->setID("combo-label");
    m_scoreLayer->addChild(m_comboLabel);

    m_hiComboLabel = cocos2d::CCLabelBMFont::create("...", fontString.c_str());
    m_hiComboLabel->setID("hi-combo-label");
    m_hiComboLabel->setScale(.4f);
    m_hiComboLabel->setPositionY(-24.f);
    m_hiComboLabel->setOpacity(176);
    m_scoreLayer->addChild(m_hiComboLabel);

    addChild(m_scoreLayer);
    m_hiCombo = geode::Mod::get()->getSavedValue<int>("hi-combo", 0);
    updateComboShit();

    m_players.reserve(20); // performance! (not really)

    scheduleUpdate();

#ifdef DEBUG_DRAW_NODE
    m_debugNode = cocos2d::CCDrawNode::create();
    m_debugNode->init();
    m_debugNode->setZOrder(999999);
    addChild(m_debugNode);
#endif

    auto currentTime = std::time(0);
    auto timeInfo = fmt::localtime(currentTime);
    // months are zero indexed, days are one indexed
    if (timeInfo.tm_mday == 1 && timeInfo.tm_mon == 3) {
        m_isAprilFools = true;
        geode::log::info("April fools!");
    }

    setID("ninja-swipe-layer"_spr);

    return true;
}

bool NinjaSwipeLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    m_numTouches++;
    if (m_numTouches != 1) return false;

    // by not clearing the point list before this adds the point causes the end
    // of the last swipe to connect to the next swipe but nobody is going to
    // notice unless you have speedhack on and in that case youll see that the
    // trail actually folds in on itself and caves in in some circumstances oh
    // god ive said too much ok goodbye good luck with the rest of the code

    m_swipe->addPoint(touch->getLocation());
    return true; // claim touch
}

void NinjaSwipeLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    m_numTouches++;
    if (m_numTouches != 1) return;

    auto points = m_swipe->m_points;
    if (points.size() != 0) {
        auto lastPoint = points[points.size() - 1].m_location;
        if (lastPoint.getDistanceSq(touch->getLocation()) < 1.f) return; // too close smh stop being stupid
    }

    m_swipe->addPoint(touch->getLocation());
    
    if (points.size() > 1) {
        float totalDistance = 0.f;
        for (int i = 1; totalDistance < 50.f && i < points.size() - 1; i++) {
            auto lastPoint = points[points.size() - i - 1].m_location;
            auto thisPoint = points[points.size() - i].m_location;
            totalDistance += lastPoint.getDistanceSq(thisPoint);
            checkSwipeIntersection(lastPoint, thisPoint);
        }
    }
}

void NinjaSwipeLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    m_numTouches++;
    if (m_numTouches != 1) return;

    m_swipe->addPoint(touch->getLocation());
}

void NinjaSwipeLayer::checkSwipeIntersection(const cocos2d::CCPoint& from, const cocos2d::CCPoint& to) {
    if (m_isBombCurrentlyExploding) return;

    for (auto player : m_players) {
        if (!player || player->retainCount() == 0) {
            ninja::log::warn("player is nullptr!");
            continue;
        }

        float radius = player->getRadius();
        if (!geode::Mod::get()->getSettingValue<bool>("disable-margin") && player->m_type != MenuIconType::Bomb) {
            float margin = 5.f;
            radius += margin * 2.f;
        }
        
        if (lineIntersectsCircle(player->getPosition(), radius, from, to)) {
            // ok so time to kill muahaha
            killPlayer(player);
        }

        if (m_isBombCurrentlyExploding) break; // couldve been set by killPlayer
    }
}

// im sure i could do this myself,
// but chatgpt is easier but WAIT it's not for actual mod code this is for a thing
// that someone smarter than me figured out in the past that im just using now
bool NinjaSwipeLayer::lineIntersectsCircle(const cocos2d::CCPoint& circleCenter, const float circleRadius, const cocos2d::CCPoint& from, const cocos2d::CCPoint& to) {
    // Vector from 'from' to 'to'
    cocos2d::CCPoint d = to - from;
    // Vector from 'from' to circle center
    cocos2d::CCPoint f = from - circleCenter;

    float a = d.dot(d);              // d·d (squared magnitude of d)
    float b = 2 * f.dot(d);          // 2 * (f·d)
    float c = f.dot(f) - circleRadius * circleRadius; // f·f - r²

    // Solve the quadratic equation: at² + bt + c = 0
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        // No intersection
        return false;
    }

    // Check if the intersection points are within the line segment
    discriminant = sqrt(discriminant);
    float t1 = (-b - discriminant) / (2 * a);
    float t2 = (-b + discriminant) / (2 * a);

    return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
}

void NinjaSwipeLayer::killPlayer(MenuIcon* player) {
    if (player->m_type == MenuIconType::Bomb) {
        ninja::log::info("bomb!");
        m_isBombCurrentlyExploding = true;
        player->m_isBombExploding = true;
        if (geode::Mod::get()->getSettingValue<bool>("particles")) player->m_particles->removeFromParent();
        FMODAudioEngine::sharedEngine()->playEffect("kablooey.wav"_spr, 1.0f, 0.0f, geode::Mod::get()->getSettingValue<double>("sfx-volume"));

        auto flash = CCLightFlash::create();
        // https://github.com/gd-hyperdash/Cocos2Dx/blob/master/cocos2dx/extensions/RobTop/CCLightFlash.h
        // for param names
        // some copied from PlayLayer::showCompleteEffect
        auto size = cocos2d::CCDirector::sharedDirector()->getWinSize();
        float screenDiagonalSize = sqrtf(size.width*size.width + size.height*size.height) + 30.f;
        flash->playEffect(
            /* pos */ player->getPosition(),
            /* col */ { 255, 255, 255 },
            /* bW */ 1.f,
            /* bwVar */ 0.f,
            /* tW */ 30.f,
            /* tWVar */ 25.f,
            /* tH */ screenDiagonalSize,
            /* dur */ .4f,
            /* durVar */ .24f,
            /* stripInterval */ .08f,
            /* stripDelay */ .13f,
            /* stripDelayVar */ .2f,
            /* rotation */ 0.f, // (doesnt matter)
            /* rotationVar */ 180.f,
            /* opacity */ 155.f,
            /* opacityVar */ 100.f,
            /* lightStripCount */ 14.f,
            /* circleRotation */ false,
            /* fadeIn */ false,
            /* delay */ 0.1f
        );
        flash->setZOrder(-10);
        addChild(flash);

        auto shake = cocos2d::CCSequence::createWithTwoActions(
            cocos2d::CCMoveTo::create(0.05f, { -5.f, 0.f }),
            cocos2d::CCMoveTo::create(0.05f, { 5.f, 0.f })
        );

        player->m_bombSprite->runAction(
            cocos2d::CCSpawn::create(
                cocos2d::CCScaleBy::create(1.6f, 1.6f),
                cocos2d::CCRepeat::create(shake, 69420),
                CCBrighten::create(1.6f),
                nullptr
            )
        );

        runAction(
            cocos2d::CCSequence::createWithTwoActions(
                cocos2d::CCDelayTime::create(1.6f),
                geode::cocos::CallFuncExt::create([this, player]{
                    if (m_isAprilFools) {
                        auto label = cocos2d::CCLabelBMFont::create("April Fools!", "bigFont.fnt");
                        label->setRotation(std::sin(rand()) * 20);
                        label->setPosition(player->getPosition());
                        label->setID("april-fools-label");
                        label->setScale(0.f);
                        addChild(label);

                        label->runAction(
                            cocos2d::CCSequence::create(
                                cocos2d::CCScaleTo::create(.2f, .9f),
                                cocos2d::CCDelayTime::create(1.f),
                                cocos2d::CCFadeOut::create(1.f),
                                cocos2d::CCRemoveSelf::create(),
                                nullptr
                            )
                        );
                    }

                    // spawn particles and delete bomb
                    spawnPlayerExplosionParticles(player->getPosition(), { 65, 64, 66 });
                    resetCombo();
                    removePlayer(player);
                    m_isBombCurrentlyExploding = false;

                    if (geode::Mod::get()->getSettingValue<bool>("flashbang")) {
                        auto layer = FlashbangLayer::create();
                        layer->addSelfToScene();
                        layer->flashAndRemove();
                    }

                    startShake();
                })
            )
        );
    } else {
        m_combo++; // increment combo if this isnt a bomb
        updateComboShit();

        // taken from MenuGameLayer::killPlayer - thanks prevter for decomp
        if (!geode::Mod::get()->getSettingValue<bool>("disable-stats-increment"))
            GameStatsManager::sharedState()->incrementStat("9", 1);
        // @geode-ignore(unknown-resource)
        FMODAudioEngine::sharedEngine()->playEffect("explode_11.ogg", 1.0f, 0.0f, geode::Mod::get()->getSettingValue<double>("sfx-volume"));
        
        spawnPlayerExplosionParticles(player->getPosition(), player->m_playerObject->m_playerColor1);
        removePlayer(player);
    }

    // state check is redundant but here for readability
    if (m_state == State::Default && geode::Mod::get()->getSettingValue<bool>("enable-gameplay")) {
        enterGameplay();
    }
}

void NinjaSwipeLayer::spawnPlayerExplosionParticles(const cocos2d::CCPoint& pos, const cocos2d::ccColor3B& col) {
    auto particles = cocos2d::CCParticleSystemQuad::create("explodeEffect.plist", false);
    particles->setPositionType(cocos2d::kCCPositionTypeGrouped);
    addChild(particles, 1);
    particles->setAutoRemoveOnFinish(true);
    particles->setPosition(pos);
    particles->setStartColor({ col.r / 255.f, col.g / 255.f, col.b / 255.f, 1.0f });
    particles->resetSystem();
    
    auto circleWave = CCCircleWave::create(10.f, 90.f, 0.5f, false);
    circleWave->m_color = col;
    circleWave->setPosition(pos);
    addChild(circleWave, 0);
}

void NinjaSwipeLayer::update(float dt) {
    m_numTouches = 0; // reset # of touches this frame
    m_swipe->setVisible(!m_isBombCurrentlyExploding);

    // if someone focused off of the game
    if (dt > .5f) {
        dt = 0.016f;
    }

    // physics
    std::vector<MenuIcon*> playersToRemove = {};
    for (auto player : m_players) {
        if (player->m_isBombExploding) continue;
        player->setPosition(player->getPosition() + (player->m_speed * dt));
        player->m_speed.y -= m_gravity * dt;
        player->setRotation(player->getRotation() + (player->m_speed.x * player->m_rotationSpeed * dt));

        auto dir = cocos2d::CCDirector::sharedDirector();
        if (player->getPositionY() <= -50.f || player->getPositionX() <= -50.f || player->getPositionX() >= dir->getScreenRight() + 50.f) {
            ninja::log::info("icon fell offscreen :broken_heart:");
            playersToRemove.push_back(player);

            // offscreen, below starting point, remove and reset combo
            // though not when bomb is exploding (for added tension)
            if (player->m_type != MenuIconType::Bomb && !m_isBombCurrentlyExploding) {
                resetCombo();
            }
        }
    }

    for (auto player : playersToRemove) {
        removePlayer(player);
    }

    if (m_players.size() == 0 && !m_waitingForNextSpawn && !m_isSendingOutSpree && !m_isBombCurrentlyExploding) {
        ninja::log::info("ran out! wait 1.5 secs, ill spawn some more");
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

#ifdef DEBUG_DRAW_NODE
    m_debugNode->clear();
    for (auto player : m_players) {
        float radius = player->getRadius();
        auto origin = player->getPosition();
        cocos2d::ccColor4F col;
        if (player->m_type == MenuIconType::Bomb) col = { 1.f, 0.f, 0.f, 1.f };
        else col = { 0.f, 0.f, 1.f, 1.f };
        m_debugNode->drawCircle(origin, radius, { 0.f, 0.f, 0.f, 0.f }, 0.5f, col, 32); // no fill
    }

    auto points = m_swipe->m_points;
    if (points.size() > 2) {
        float distance = 0.f;
        for (int i = 1; distance < 50.f && i < points.size() - 1; i++) {
            auto lastPoint = points[points.size() - i - 1].m_location;
            auto thisPoint = points[points.size() - i].m_location;
            distance += lastPoint.getDistanceSq(thisPoint);
            m_debugNode->drawSegment(lastPoint, thisPoint, 1.f, { 0.f, 1.f, 0.f, 1.f });
        }
    }
#endif

    updateShake(dt);
}

void NinjaSwipeLayer::updateShake(float dt) {
    // One Life makes menulayer nullptr while you're on it and breaks stuff
    if (!MenuLayer::get()) return;

    if (m_shakeTick <= 0) {
        MenuLayer::get()->setPosition({ 0.f, 0.f });
        return;
    }

    m_shakeTick -= dt;
    float shakePercentage = m_shakeTick / m_maxShakeTick;
    
    float offsetX = ninja::random::g_shakeMovementDistribution(ninja::random::g_gen) * 12.f * shakePercentage;
    float offsetY = ninja::random::g_shakeMovementDistribution(ninja::random::g_gen) * 12.f * shakePercentage;

    MenuLayer::get()->setPosition({ offsetX, offsetY });
}

void NinjaSwipeLayer::startShake() {
    if (GameManager::get()->getGameVariable("0172")) return;

    // I stay out too late
    // Got nothing in my brain
    // That's what people say, mm-mm
    // That's what people say, mm-mm

    // I go on too many dates
    // But I can't make 'em stay
    // At least that's what people say, mm-mm
    // That's what people say, mm-mm

    // But I keep cruisin'
    // Can't stop, won't stop movin'
    // It's like I got this music in my mind
    // Sayin' it's gonna be alright

    // 'Cause the players gonna play, play, play, play, play
    // And the haters gonna hate, hate, hate, hate, hate
    // Baby, I'm just gonna shake, shake, shake, shake, shake
    // I shake it off, I shake it off (hoo-hoo-hoo)

    m_shakeTick = m_maxShakeTick;
    ninja::log::info("shakey bakey");
}

void NinjaSwipeLayer::removePlayer(MenuIcon* player) {
    auto location = std::find(m_players.begin(), m_players.end(), player);
    m_players.erase(location);
    player->removeFromParent();
    
}

void NinjaSwipeLayer::enterGameplay() {
    if (m_state == State::Gameplay) return;

    m_state = State::Gameplay;
    ninja::log::info("NINJA TIME BABY");
    auto menuLayer = static_cast<HookedMenuLayer*>(MenuLayer::get());
    menuLayer->runEnterGameplayAnimations();

    m_scoreLayer->runAction(cocos2d::CCFadeIn::create(.9f));
    m_exitButton->runAction(cocos2d::CCSpawn::createWithTwoActions(
        cocos2d::CCEaseBackOut::create(cocos2d::CCMoveBy::create(.9f, {0.f, -50.f})),
        cocos2d::CCFadeIn::create(.9f)
    ));

    m_exitButton->setEnabled(true);

    FMODAudioEngine::sharedEngine()->fadeOutMusic(1.f, 0);
    FMODAudioEngine::sharedEngine()->playEffect("gamestart.wav"_spr, 1.f, 0.f, geode::Mod::get()->getSettingValue<double>("sfx-volume"));
}

void NinjaSwipeLayer::exitGameplay(cocos2d::CCObject* sender) {
    if (m_state == State::Default) return;

    m_state = State::Default;
    ninja::log::info("no more ninja time :broken_heart:");
    auto menuLayer = static_cast<HookedMenuLayer*>(MenuLayer::get());
    menuLayer->runExitGameplayAnimations();
    
    m_scoreLayer->runAction(cocos2d::CCFadeOut::create(.9f));
    m_exitButton->runAction(cocos2d::CCSpawn::createWithTwoActions(
        cocos2d::CCEaseBackOut::create(cocos2d::CCMoveBy::create(.9f, {0.f, 50.f})),
        cocos2d::CCFadeOut::create(.9f)
    ));

    m_exitButton->setEnabled(false);

    FMODAudioEngine::sharedEngine()->fadeInMusic(1.f, 0);
    FMODAudioEngine::sharedEngine()->playEffect("gameover.wav"_spr, 1.f, 0.f, geode::Mod::get()->getSettingValue<double>("sfx-volume"));
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

void NinjaSwipeLayer::resetCombo() {
    m_combo = 0;
    m_comboAnimationPlayed = false;
    updateComboShit();
    geode::Mod::get()->setSavedValue<int>("hi-combo", m_hiCombo);
}

void NinjaSwipeLayer::spawnPlayers() {
    if (m_players.size() != 0) return;

    int type;
    if (m_isAprilFools) {
        type = 1; // always spree
    } else {
        // new random type that isnt m_lastSpawnType
        do {
            type = ninja::random::g_spawnTypeDistribution(ninja::random::g_gen);
        } while (type == m_lastSpawnType);
    }

    ninja::log::info("spawning players (type: {})", type);
    m_lastSpawnType = type;

    switch(type) {
        case 0: {
            // all go to the same height
            float sharedHeight = ninja::random::g_launchSpeedYDistribution(ninja::random::g_gen);
            int count = ninja::random::g_playerSpawnDistribution(ninja::random::g_gen);
            for (int i = 0; i < count; i++) {
                auto player = MenuIcon::create(static_cast<MenuIconType>(ninja::random::g_menuIconTypeDistribution(ninja::random::g_gen)));
                player->m_speed.y = sharedHeight;
                m_players.push_back(player);
            }
            break;
        }

        case 1: {
            // spree, bunch get sent out randomly in quick succession
            int count = ninja::random::g_spreeIconCountDistribution(ninja::random::g_gen);
            m_isSendingOutSpree = true;

            // alright guys lets cocos action this up
            auto spawnSequence = cocos2d::CCSequence::createWithTwoActions(
                geode::cocos::CallFuncExt::create([this]{
                    auto type = m_isAprilFools ? MenuIconType::Bomb : MenuIconType::Player;
                    auto icon = MenuIcon::create(type);
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

        case 2: {
            // random # of bombs
            int count = ninja::random::g_bombSpawnDistribution(ninja::random::g_gen);
            for (int i = 0; i < count; i++) {
                m_players.push_back(MenuIcon::create(MenuIconType::Bomb));
            }
            break;
        }

        case 3:
        case 4: {
            // random # of icons
            int count = ninja::random::g_playerSpawnDistribution(ninja::random::g_gen);
            for (int i = 0; i < count; i++) {
                m_players.push_back(MenuIcon::create(MenuIconType::Player));
            }
            break;
        }

        case 5:
        case 6: {
            // random mix
            int count = ninja::random::g_mixIconCountDistribution(ninja::random::g_gen);
            for (int i = 0; i < count; i++) {
                m_players.push_back(MenuIcon::create(static_cast<MenuIconType>(ninja::random::g_menuIconTypeDistribution(ninja::random::g_gen))));
            }
            break;
        }
    }

    for (auto player : m_players) {
        addChild(player);
    }
}

$on_mod(Loaded) {
    ninja::log::info("hi spaghettdev");
}
