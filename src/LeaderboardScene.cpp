#include "LeaderboardScene.hpp"

LeaderboardScene* LeaderboardScene::create() {
    auto ret = new LeaderboardScene;
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

cocos2d::CCScene* LeaderboardScene::scene() {
    auto ret = cocos2d::CCScene::create();
    ret->addChild(LeaderboardScene::create());
    return ret;
}

void LeaderboardScene::keyBackClicked() { cocos2d::CCDirector::sharedDirector()->popSceneWithTransition(0.5f, cocos2d::PopTransition::kPopTransitionFade); }
void LeaderboardScene::keyBackClicked(cocos2d::CCObject*) { keyBackClicked(); }

bool LeaderboardScene::init() {
    if (!CCLayer::init()) return false;

    setKeypadEnabled(true);

    auto mainMenu = cocos2d::CCMenu::create();
    mainMenu->setPosition({ 0.f, 0.f });
    addChild(mainMenu);

    // not from creation rotation i promise
    auto size = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto background = cocos2d::CCSprite::create("GJ_gradientBG.png");
    background->setScaleX( size.width / background->getContentWidth() );
    background->setScaleY( size.height / background->getContentHeight() );
    background->setAnchorPoint({ 0, 0 });
    background->setColor({ 0, 102, 255 });
    background->setZOrder(-10);
    addChild(background);

    auto closeBtnSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    auto closeBtn = CCMenuItemSpriteExtra::create( closeBtnSprite, this, menu_selector(LeaderboardScene::keyBackClicked) );
    closeBtn->setPosition({ 25, size.height - 25 });
    mainMenu->addChild(closeBtn);

    geode::addSideArt(this, geode::SideArt::Bottom);
    geode::addSideArt(this, geode::SideArt::TopRight);

    m_mainLayer = cocos2d::CCLayer::create();
    m_mainLayer->setID("main-layer");
    addChild(m_mainLayer);

    return true;
}
