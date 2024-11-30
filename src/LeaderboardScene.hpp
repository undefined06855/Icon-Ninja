#pragma once
#include <Geode/Geode.hpp>

class LeaderboardScene : public cocos2d::CCLayer {
public:
    static LeaderboardScene* create();
    static cocos2d::CCScene* scene();
    bool init();

    cocos2d::CCLayer* m_mainLayer;

    void keyBackClicked();
    void keyBackClicked(cocos2d::CCObject*);
};
