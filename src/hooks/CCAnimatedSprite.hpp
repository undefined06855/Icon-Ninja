#pragma once
#include <Geode/modify/CCAnimatedSprite.hpp>

class $modify(HookedCCAnimatedSprite, CCAnimatedSprite) {
    void runAnimation(gd::string animID);
    std::string getReplacementAnimID(std::string origID);
};
