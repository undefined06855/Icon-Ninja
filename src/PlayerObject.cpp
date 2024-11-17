#include "PlayerObject.hpp"

void HookedPlayerObject::hitGround(GameObject* p0, bool p1) {
    if (geode::Mod::get()->getSettingValue<bool>("custom-player-movement") && !GJBaseGameLayer::get()) {
        // if there';s a playerobject not in gjbgl then most likely we're in
        // menulayer and need to cancel hitGround calls
        return;
    }

    PlayerObject::hitGround(p0, p1);
}
