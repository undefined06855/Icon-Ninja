#pragma once
#include <Geode/binding/CCSpriteWithHue.hpp>

class CCSpriteWithLuminosity : public CCSpriteWithHue {
public:
    static CCSpriteWithLuminosity* createWithSpriteFrame(cocos2d::CCSpriteFrame* frame);

    gd::string getShaderName();
    const char* shaderBody();
    void setFixedLuminance(float value);
};
