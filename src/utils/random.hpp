#pragma once
#include <random>

namespace ninja {
namespace random {

extern std::mt19937 gen;
extern std::uniform_real_distribution<float> startXDistribution;
extern std::uniform_real_distribution<float> launchSpeedXDistribution;
extern std::uniform_real_distribution<float> launchSpeedYDistribution;
extern std::uniform_real_distribution<float> rotationSpeedDistribution;
extern std::uniform_int_distribution<int> spawnTypeDistribution;
extern std::uniform_int_distribution<int> playerSpawnDistribution;
extern std::uniform_int_distribution<int> bombSpawnDistribution;
extern std::uniform_int_distribution<int> menuIconTypeDistribution;
extern std::uniform_int_distribution<int> spreeIconCountDistribution;
extern std::uniform_int_distribution<int> mixIconCountDistribution;

void randomisePlayerObject(PlayerObject* player);

}
}
