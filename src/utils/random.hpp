#pragma once
#include <random>

namespace ninja::random {

extern std::mt19937 g_gen;
extern std::uniform_real_distribution<float> g_startXDistribution;
extern std::uniform_real_distribution<float> g_launchSpeedXDistribution;
extern std::uniform_real_distribution<float> g_launchSpeedYDistribution;
extern std::uniform_real_distribution<float> g_rotationSpeedDistribution;
extern std::uniform_int_distribution<int> g_spawnTypeDistribution;
extern std::uniform_int_distribution<int> g_playerSpawnDistribution;
extern std::uniform_int_distribution<int> g_bombSpawnDistribution;
extern std::uniform_int_distribution<int> g_menuIconTypeDistribution;
extern std::uniform_int_distribution<int> g_spreeIconCountDistribution;
extern std::uniform_int_distribution<int> g_mixIconCountDistribution;
extern std::uniform_real_distribution<float> g_shakeMovementDistribution;

PlayerObject* createRandomPlayerObject();
void randomisePlayerObject(PlayerObject* player);

}
