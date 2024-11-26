#include <Geode/Geode.hpp>
#include "random.hpp"

namespace ninja {
namespace random {

std::random_device seed;
std::mt19937 gen = std::mt19937(seed());
std::uniform_real_distribution<float> startXDistribution = std::uniform_real_distribution<float>(70.f, cocos2d::CCDirector::sharedDirector()->getScreenRight() - 70.f);
std::uniform_real_distribution<float> launchSpeedXDistribution = std::uniform_real_distribution<float>(0.f, 200.f); // could become 0 to -200 depending on start side
std::uniform_real_distribution<float> launchSpeedYDistribution = std::uniform_real_distribution<float>(340.0, 400.f); 
std::uniform_real_distribution<float> rotationSpeedDistribution = std::uniform_real_distribution<float>(-6.f, 6.f); 
std::uniform_int_distribution<int> spawnTypeDistribution = std::uniform_int_distribution<int>(0, 6); // same height, spree, bomb one, bomb two, random one, random two, mix
std::uniform_int_distribution<int> playerSpawnDistribution = std::uniform_int_distribution<int>(1, 4);
std::uniform_int_distribution<int> bombSpawnDistribution = std::uniform_int_distribution<int>(1, 3);
std::uniform_int_distribution<int> menuIconTypeDistribution = std::uniform_int_distribution<int>(0, 1); // 0 = MenuIconType::Player  1 = MenuIconType::Bomb
std::uniform_int_distribution<int> spreeIconCountDistribution = std::uniform_int_distribution<int>(4, 15); // should be a bit more than normal
std::uniform_int_distribution<int> mixIconCountDistribution = std::uniform_int_distribution<int>(3, 5); // should be a bit more than normal


// reimplemented from MenuGameLayer::resetPlayer
// it kinda broke when i tried to use it without using the menugamelayer's 
// m_playerObject so thanks to prevter for remaking it well enough so that it can
// be used in this silly joke mod
// https://discord.com/channels/911701438269386882/911702535373475870/1310683736152477708
// most things specific to menugamelayer have been removed and the rest cleaned up
// but actual logic is kept identical
void randomisePlayerObject(PlayerObject* player) {
    player->m_hasGlow = (float)rand() / RAND_MAX > 0.8;
    player->setColor(GameManager::sharedState()->colorForIdx((float)rand() / RAND_MAX * 108.0));
    player->setSecondColor(GameManager::sharedState()->colorForIdx((float)rand() / RAND_MAX * 108.0));

    float randFloat = (float)rand() / RAND_MAX;
    if (randFloat < 0.12) {
        player->toggleFlyMode(true, false);
        int shipFrame = (float)rand() / RAND_MAX * 169.0;
        player->updatePlayerShipFrame(shipFrame);
    } else if (randFloat < 0.24) {
        player->toggleRollMode(true, false);
        int rollFrame = (float)rand() / RAND_MAX * 118.0;
        if (rollFrame <= 1) rollFrame = 1;
        player->updatePlayerRollFrame(rollFrame);
    } else if (randFloat < 0.36) {
        player->toggleBirdMode(true, false);
        int birdFrame = (float)rand() / RAND_MAX * 149.0;
        player->updatePlayerBirdFrame(birdFrame);
    } else if (randFloat < 0.48) {
        player->toggleDartMode(true, false);
        int dartFrame = (float)rand() / RAND_MAX * 96.0;
        player->updatePlayerDartFrame(dartFrame);
    } else if (randFloat < 0.6) {
        player->toggleRobotMode(true, false);
        int robotFrame = (float)rand() / RAND_MAX * 68.0;
        player->updatePlayerRobotFrame(robotFrame);
    } else if (randFloat < 0.7) {
        player->toggleSpiderMode(true, false);
        int spiderFrame = (float)rand() / RAND_MAX * 69.0;
        player->updatePlayerSpiderFrame(spiderFrame);
    } else if (randFloat < 0.8) {
        player->toggleSwingMode(true, false);
        int swingFrame = (float)rand() / RAND_MAX * 43.0;
        player->updatePlayerSwingFrame(swingFrame);
    } else {
        int frame = (float)rand() / RAND_MAX * 485.0;
        if (frame < 1) frame = 1;
        player->updatePlayerFrame(frame);
        player->switchedToMode(GameObjectType::CubePortal);
    }

    player->togglePlayerScale((float)rand() / RAND_MAX <= 0.1, false);
    player->updateGlowColor();

    float timeMod = 0;
    randFloat = (float)rand() / RAND_MAX;

         if (randFloat < 0.2)  timeMod = 1.3f;
    else if (randFloat < 0.4)  timeMod = 1.1f;
    else if (randFloat < 0.6)  timeMod = 0.7f;
    else if (randFloat < 0.65) timeMod = 1.6f;
    else                       timeMod = 0.9f;
    
    player->updateTimeMod(timeMod, false);
    player->updateEffects(0.f);
}

}
}
