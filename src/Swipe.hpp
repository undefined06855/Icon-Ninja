#pragma once
#include <Geode/cocos/include/cocos2d.h>
#include "SwipePoint.hpp"

class Swipe : public cocos2d::CCNode {
public:
    static Swipe* create(cocos2d::CCTexture2D* texture);
    bool init(cocos2d::CCTexture2D* texture);

    cocos2d::CCGLProgram* m_program;
    cocos2d::CCPoint m_lastPoint; // used to calculate angle
    std::vector<SwipePoint*> m_points;
    cocos2d::CCTexture2D* m_texture;

    void update(float dt);
    void draw();
    void addPoint(cocos2d::CCPoint point);
};
