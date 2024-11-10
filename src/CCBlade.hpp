#pragma once
#include <Geode/Geode.hpp>

// https://github.com/hiepnd/CCBlade/blob/master/CCBlade.h
// Converted by GPT-4o, touched up by undefined0 (me)

#define USE_LAGRANGE 1
#define USE_UPDATE_FOR_POP 1
#define POP_TIME_INTERVAL (1.0f / 60.0f)

class CCBlade : public cocos2d::CCNode {
public:
    static CCBlade* createWithMaximumPoint(int limit);

    bool initWithMaximumPoint(int limit);
    virtual ~CCBlade() = default;

    void push(const cocos2d::CCPoint& v);
    void pop(int n);
    void clear();
    void reset();
    void dim(bool dim);
    void finish();
    void setWidth(float newWidth) { _width = newWidth; }

    virtual void update(float dt) override;
    virtual void draw() override;

    void populateVertices();
    void shift();

    unsigned int _pointLimit;
    float _width = 5;
    bool _reset = false;
    bool _finish = false;
    bool _willPop = false;
    float timeSinceLastPop = 0.f;
    float popTimeInterval = POP_TIME_INTERVAL;

    std::vector<cocos2d::CCPoint> _path;
    std::vector<cocos2d::CCPoint> vertices;
    std::vector<cocos2d::CCPoint> coordinates;
    cocos2d::CCTexture2D* _texture;
};

// Utility functions (inline functions from Objective-C)
inline float fangle(const cocos2d::CCPoint& vect) {
    if (vect.x == 0.0 && vect.y == 0.0) return 0;
    if (vect.x == 0.0) return vect.y > 0 ? M_PI / 2 : -M_PI / 2;
    if (vect.y == 0.0 && vect.x < 0) return -M_PI;

    float angle = atan(vect.y / vect.x);
    return vect.x < 0 ? angle + M_PI : angle;
}

inline void f1(const cocos2d::CCPoint& p1, const cocos2d::CCPoint& p2, float d, cocos2d::CCPoint* o1, cocos2d::CCPoint* o2) {
    float l = ccpDistance(p1, p2);
    float angle = fangle(p2 - p1);
    *o1 = p1 + cocos2d::CCPoint{l, d}.rotateByAngle(p1, angle);
    *o2 = p1 + cocos2d::CCPoint{l, -d}.rotateByAngle(p1, angle);
}
