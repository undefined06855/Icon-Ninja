#pragma once
#include <Geode/Geode.hpp>

enum class MenuIconType {
    Player = 0, Bomb = 1
};

class MenuIcon : public cocos2d::CCNode {
public:
    static MenuIcon* create(MenuIconType type);
    bool init(MenuIconType type);
    void initialiseValues();
    
    cocos2d::CCPoint m_speed = {0.f, 0.f};
    float m_rotationSpeed = 0.f;

    PlayerObject* m_playerObject = nullptr;    // only one of these two will be
    cocos2d::CCSprite* m_bombSprite = nullptr; // nullptr and one will be valid
    MenuIconType m_type;
    
    bool m_isBombExploding = false;

    cocos2d::CCPoint getWorldPos();
    float getRadius();
};
