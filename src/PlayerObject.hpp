#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

class $modify(HookedPlayerObject, PlayerObject) {
    void updateSwingFire();
};
