#pragma once
#include "CCSpriteWithLuminosity.hpp"

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

    // either playerobject or bombSprite and particles will be valid
    // one or the other, never both
    PlayerObject* m_playerObject = nullptr;
    CCSpriteWithLuminosity* m_bombSprite = nullptr;
    cocos2d::CCParticleSystemQuad* m_particles = nullptr;
    MenuIconType m_type;
    
    bool m_isBombExploding = false;

    cocos2d::CCPoint getWorldPos();
    float getRadius();
};
