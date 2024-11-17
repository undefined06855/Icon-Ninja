#include "GameStatsManager.hpp"

void HookedGameStatsManager::incrementStat(const char* stat, int amount) {
    if (geode::Mod::get()->getSettingValue<bool>("disable-stats-increment") && strcmp(stat, "9") == 0) {
        return;
    }

    GameStatsManager::incrementStat(stat, amount);
}
