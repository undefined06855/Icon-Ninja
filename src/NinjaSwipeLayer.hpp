#pragma once
#include "Swipe.hpp"
#include "MenuIcon.hpp"

enum class State {
    Default = 0, Gameplay
};

class NinjaSwipeLayer : public cocos2d::CCLayer {
public:
    static NinjaSwipeLayer* create();
    bool init() override;

#ifdef DEBUG_DRAW_NODE
    cocos2d::CCDrawNode* m_debugNode;
#endif

    Swipe* m_swipe;

    std::vector<MenuIcon*> m_players;
    State m_state = State::Default;

    bool m_isBombCurrentlyExploding = false;

    bool m_isSendingOutSpree = false;
    int m_lastSpawnType = 1; // spree is 1 - so spree will never be the first one

    const float m_gravity = 315;

    int m_combo = 0;
    int m_hiCombo = 0;
    bool m_comboAnimationPlayed = false;

    bool m_waitingForNextSpawn = false;

    CCMenuItemSpriteExtra* m_exitButton = nullptr;
    cocos2d::CCLabelBMFont* m_comboLabel = nullptr;
    cocos2d::CCLabelBMFont* m_hiComboLabel = nullptr;
    cocos2d::CCLayerRGBA* m_scoreLayer = nullptr;

    float m_shakeTick = 0.f;
    const float m_maxShakeTick = 0.7f;

    bool m_isAprilFools = false;

    void ccTouchesBegan(cocos2d::CCSet* touch, cocos2d::CCEvent* event) override;
    void ccTouchesMoved(cocos2d::CCSet* touch, cocos2d::CCEvent* event) override;
    void ccTouchesEnded(cocos2d::CCSet* touch, cocos2d::CCEvent* event) override;
    void update(float dt) override;
    void updateShake(float dt);

    void checkSwipeIntersection(const cocos2d::CCPoint& from, const cocos2d::CCPoint& to);
    bool lineIntersectsCircle(const cocos2d::CCPoint& circleCenter, const float circleRadius, const cocos2d::CCPoint& from, const cocos2d::CCPoint& to);

    void killPlayer(MenuIcon* player);
    void removePlayer(MenuIcon* player);
    void spawnPlayers();
    void spawnPlayerExplosionParticles(const cocos2d::CCPoint& pos, const cocos2d::ccColor3B& col);
    void exitGameplay(CCObject* sender);
    void enterGameplay();
    void updateComboShit();
    void resetCombo();
    void startShake();
};
