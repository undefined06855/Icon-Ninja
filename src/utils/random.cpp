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

}
}
