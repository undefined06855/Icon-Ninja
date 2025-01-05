#include "PlayerObject.hpp"

void HookedPlayerObject::updateSwingFire() {
    // see if we're in menulayer
    if (!cocos2d::CCScene::get()->getChildByType<MenuLayer>(0)) {
        PlayerObject::updateSwingFire();
        return;
    }

    // show all swing fires regardless if we're in menulayer
    m_swingFireTop->animateFireIn();
    m_swingFireBottom->animateFireIn();
    m_swingBurstParticles1->resumeSystem();
    m_swingBurstParticles2->resumeSystem();
}
