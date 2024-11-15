#pragma once
#include <Geode/Geode.hpp>

struct SwipePoint {
    cocos2d::CCPoint location;
    float direction;
    float size = 0.15f;

    bool operator==(const SwipePoint& other) {
        return
            location == other.location
         && direction == other.direction
         && size == other.size;
    }

    std::pair<cocos2d::CCPoint, cocos2d::CCPoint> calculatePointPositions() {
        float angle1 = fmod(direction + 1.5708f, 2.f * M_PI);
        float angle2 = fmod(direction - 1.5708f, 2.f * M_PI);

        float dist = 50.f * size;

        cocos2d::CCPoint p1 = location + cocos2d::CCPoint{ cos(angle1)*dist, sin(angle1)*dist };
        cocos2d::CCPoint p2 = location + cocos2d::CCPoint{ cos(angle2)*dist, sin(angle2)*dist };
        return { p1, p2 };
    }
};

class Swipe : public cocos2d::CCNode {
public:
    static Swipe* create(cocos2d::CCTexture2D* texture);
    bool init(cocos2d::CCTexture2D* texture);

    cocos2d::CCGLProgram* m_program;
    cocos2d::CCPoint m_lastPoint;
    std::vector<SwipePoint> m_points;
    cocos2d::CCTexture2D* m_texture;

    void update(float dt);
    void draw();
    void addPoint(cocos2d::CCPoint point);
};
