#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/GameStatsManager.hpp>

class $modify(HookedGameStatsManager, GameStatsManager) {
    void incrementStat(const char* stat, int amount);
};
