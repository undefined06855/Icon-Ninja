#include "CCAnimatedSprite.hpp"
#include "utils/log.hpp"

void HookedCCAnimatedSprite::runAnimation(gd::string animID) {
    std::string replacement = getReplacementAnimID(animID);
    ninja::log::info("convert anim {} -> {}", animID, replacement);
    CCAnimatedSprite::runAnimation(replacement);
}

std::string HookedCCAnimatedSprite::getReplacementAnimID(std::string origID) {
    // check if we're in menulayer
    if (!cocos2d::CCScene::get()->getChildByType<MenuLayer>(0)) {
        // nevermind...
        return origID;
    }

    // replace origID with fall_loop if its a run animation
    if (origID == "run" || origID == "walk") return "fall_loop";

    return origID;
}
