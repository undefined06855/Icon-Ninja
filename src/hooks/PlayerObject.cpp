#include "PlayerObject.hpp"
#include <Geode/binding/PlayerFireBoostSprite.hpp>
#include <Geode/binding/MenuLayer.hpp>

void HookedPlayerObject::updateSwingFire() {
    // see if we're in menulayer
    if (GJBaseGameLayer::get()) {
        PlayerObject::updateSwingFire();
        return;
    }

    // show all swing fires regardless if we're in menulayer
    m_swingFireTop->animateFireIn();
    m_swingFireBottom->animateFireIn();
    m_swingBurstParticles1->resumeSystem();
    m_swingBurstParticles2->resumeSystem();
}
