#pragma once
#include "Swipe.hpp"
#include "MenuIcon.hpp"
#include <random>

enum class State {
	Default = 0, Gameplay
};

class NinjaSwipeLayer : public cocos2d::CCLayer {
public:
    static NinjaSwipeLayer* create();
    bool init() override;

    bool m_isFingerDown = false;
    Swipe* m_swipe;
    cocos2d::CCPoint m_lastSwipePoint;
    std::vector<MenuIcon*> m_players;
    State m_state = State::Default;
    bool m_isSendingOutSpree = false;
    
    const float m_gravity = 315;

    int m_combo = 0;
    int m_hiCombo = 0;
    bool m_comboAnimationPlayed = false;

    bool m_waitingForNextSpawn = false;

    CCMenuItemSpriteExtra* m_exitButton = nullptr;
    cocos2d::CCLabelBMFont* m_comboLabel = nullptr;
    cocos2d::CCLabelBMFont* m_hiComboLabel = nullptr;
    cocos2d::CCLayerRGBA* m_scoreLayer = nullptr;

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void update(float dt) override;

    void checkSwipeIntersection(const cocos2d::CCPoint& from, const cocos2d::CCPoint& to);
    bool lineIntersectsRect(const cocos2d::CCRect& rect, const cocos2d::CCPoint& p1, const cocos2d::CCPoint& p2);

    void killPlayer(MenuIcon* player);
	void spawnPlayers();
	void exitGameplay(CCObject* sender);
	void enterGameplay();
    void updateComboShit();
};
