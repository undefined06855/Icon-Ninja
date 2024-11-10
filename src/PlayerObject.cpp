#include "PlayerObject.hpp"

void HookedPlayerObject::hitGround(GameObject* p0, bool p1) {
    if (geode::Mod::get()->getSettingValue<bool>("custom-player-movement") && MenuLayer::get()) {
        return;
    }

    PlayerObject::hitGround(p0, p1);
}
