#include <Geode/binding/PlayerObject.hpp>
#include <Geode/binding/GameManager.hpp>
#include "random.hpp"
#include "log.hpp"
#include "../../include/events.hpp"

#ifdef KNOWN_PLAYERS_COMPAT
#include <iandyhd3.known_players/include/KnownPlayers.hpp>
#endif

namespace ninja {
namespace random {

// TODO: use geode random utils
std::random_device seed;
std::mt19937 g_gen = std::mt19937(seed());
auto g_startXDistribution = std::uniform_real_distribution<float>(70.f, cocos2d::CCDirector::sharedDirector()->getScreenRight() - 70.f);
auto g_launchSpeedXDistribution = std::uniform_real_distribution<float>(0.f, 200.f); // could become 0 to -200 depending on start side
auto g_launchSpeedYDistribution = std::uniform_real_distribution<float>(340.0, 400.f);
auto g_rotationSpeedDistribution = std::uniform_real_distribution<float>(-6.f, 6.f);
auto g_spawnTypeDistribution = std::uniform_int_distribution<int>(0, 6);
auto g_playerSpawnDistribution = std::uniform_int_distribution<int>(1, 4);
auto g_bombSpawnDistribution = std::uniform_int_distribution<int>(1, 3);
auto g_menuIconTypeDistribution = std::uniform_int_distribution<int>(0, 1); // 0 = MenuIconType::Player  1 = MenuIconType::Bomb
auto g_spreeIconCountDistribution = std::uniform_int_distribution<int>(4, 15); // should be a bit more than normal
auto g_mixIconCountDistribution = std::uniform_int_distribution<int>(3, 5); // should be a bit more than normal
auto g_shakeMovementDistribution = std::uniform_real_distribution<float>(-1.f, 1.f);


PlayerObject* createRandomPlayerObject() {
    auto ret = PlayerObject::create(1, 1, nullptr, cocos2d::CCLayer::create(), false);

    if (EarlyNewPlayerEvent().send(ret)) {
        // if it's been eaten, return here
        NewPlayerEvent().send(ret);
        return ret;
    }

    // known players compatibility
#ifdef KNOWN_PLAYERS_COMPAT
    if (geode::Loader::get()->isModLoaded("iandyhd3.known_players")) {
        known_players::events::NextIconModifyPlayerObject(ret).send(ret);
    } else {
        randomisePlayerObject(ret);
    }
#else
    randomisePlayerObject(ret);
#endif

    NewPlayerEvent().send(ret);

    return ret;
}

// reimplemented from MenuGameLayer::resetPlayer
// it kinda broke when i tried to use resetPlayer without using the menugamelayer's
// m_playerObject so thanks to prevter for remaking it well enough so that it can
// be used in this silly joke mod
// https://discord.com/channels/911701438269386882/911702535373475870/1310683736152477708
// most things specific to menugamelayer have been removed and the rest cleaned up
// but actual logic is kept identical so this can be plonked into any project and
// it will work fine - no icon ninja specific stuff has been overridden here
void randomisePlayerObject(PlayerObject* player) {
    player->m_hasGlow = (float)rand() / RAND_MAX > 0.8;
    player->setColor(GameManager::sharedState()->colorForIdx((float)rand() / RAND_MAX * 108.0));
    player->setSecondColor(GameManager::sharedState()->colorForIdx((float)rand() / RAND_MAX * 108.0));

    float randFloat = (float)rand() / RAND_MAX;
    if (randFloat < .12f) {
        player->toggleFlyMode(true, false);
        int shipFrame = (float)rand() / RAND_MAX * 169.f;
        player->updatePlayerShipFrame(shipFrame);
    } else if (randFloat < .24f) {
        player->toggleRollMode(true, false);
        int rollFrame = (float)rand() / RAND_MAX * 118.f;
        if (rollFrame <= 1) rollFrame = 1;
        player->updatePlayerRollFrame(rollFrame);
    } else if (randFloat < .36f) {
        player->toggleBirdMode(true, false);
        int birdFrame = (float)rand() / RAND_MAX * 149.f;
        player->updatePlayerBirdFrame(birdFrame);
    } else if (randFloat < .48f) {
        player->toggleDartMode(true, false);
        int dartFrame = (float)rand() / RAND_MAX * 96.f;
        player->updatePlayerDartFrame(dartFrame);
    } else if (randFloat < .6f) {
        player->toggleRobotMode(true, false);
        int robotFrame = (float)rand() / RAND_MAX * 68.f;
        player->updatePlayerRobotFrame(robotFrame);
    } else if (randFloat < .7f) {
        player->toggleSpiderMode(true, false);
        int spiderFrame = (float)rand() / RAND_MAX * 69.f;
        player->updatePlayerSpiderFrame(spiderFrame);
    } else if (randFloat < .8f) {
        player->toggleSwingMode(true, false);
        int swingFrame = (float)rand() / RAND_MAX * 43.f;
        player->updatePlayerSwingFrame(swingFrame);
    } else {
        int frame = (float)rand() / RAND_MAX * 485.f;
        if (frame < 1) frame = 1;
        player->updatePlayerFrame(frame);
        player->switchedToMode(GameObjectType::CubePortal);
    }

    player->togglePlayerScale((float)rand() / RAND_MAX <= .1f, false);
    player->updateGlowColor();

    float timeMod = 0.f;
    randFloat = (float)rand() / RAND_MAX;

         if (randFloat < .2f)  timeMod = 1.3f;
    else if (randFloat < .4f)  timeMod = 1.1f;
    else if (randFloat < .6f)  timeMod = 0.7f;
    else if (randFloat < .65f) timeMod = 1.6f;
    else                       timeMod = 0.9f;

    player->updateTimeMod(timeMod, false);
    player->updateEffects(0.f);
}


}
}
